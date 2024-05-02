#!/usr/bin/env python3

import sys
import pcre2
import re
from pathlib import Path
from pprint import pprint
from dataclasses import dataclass, field
import wrap_text

type_map = {
    "bool": "bool",
    "string": "std::string",
    "bytes": "std::basic_string<uint8_t>",
    "double": "double",
    "float": "float",
    "uint32": "std::uint32_t",
    "uint64": "std::uint64_t",
    "int32": "std::int32_t",
    "int64": "std::int64_t",
    "fixed32": "std::uint32_t",
    "fixed64": "std::uint64_t",
    "sfixed32": "std::int32_t",
    "sfixed64": "std::int64_t",
    "Any": None
}

# optional, repeated, oneof, map


@dataclass
class Node:
    name: str
    type: str
    body: list


@dataclass
class MemberRow:
    name: str
    type: str
    pos: int
    attr: str = None


@dataclass
class EnumRow:
    name: str
    value: int


statement_matcher = pcre2.compile(
    r'(enum|message|oneof)\s+(\w+)\s+\{(\s*((?:[^{}]*|(?R))*)\s*)\}', pcre2.G | pcre2.M)

inner_matcher = pcre2.compile(
    r'(?<ST>\s*(\w+\s+)+=\s+(\d+));|(?<BL>(?<MODE>enum|message|oneof)\s+(?<NAME>\w+)\s+\{\s*(?<BDY>((?:[^{}]*|(?R))*))\s*\})', pcre2.G | pcre2.M)


def parse_message(data):
    orig_data = data
    results = []
    while True:
        if data[0] == '\n':
            data = data[1:]
        if len(data) == 0:
            break
        if data.isspace():
            break
        body = inner_matcher.match(data)
        try:
            row = str(body['ST']).split()
            if len(row) == 3:
                results.append(EnumRow(row[0], int(row[2])))
            elif len(row) == 4:
                results.append(MemberRow(row[1], row[0], int(row[-1])))
            elif len(row) == 5:
                results.append(MemberRow(row[2], row[1], int(row[-1]), row[0]))
            data = data[body.end():]
            continue
        except:
            pass
        try:
            name = body['NAME']
            mode = body['MODE']
            bdy = body['BDY']
        except Exception as ex:
            print(f'could not parse statement or block')
            sys.exit(-1)
        parsed_body = parse_message(bdy)
        results.append(Node(name, mode, parsed_body))
        data = data[body.end():]
    return results


class ProtobufWrapper:
    def __init__(self, proto_nspace, output_name):
        self.proto_nspace = proto_nspace
        self.output_name = Path(output_name)
        self.enum_set = set()
        self.parse_tree = []

    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exec_value, exc_tb):
        hdr_path = self.output_name.with_suffix('.h')
        hdr_fh = open(hdr_path, 'w')
        src_fh = open(self.output_name.with_suffix('.cpp'), 'w')
        try:
            hdr_fh.write(wrap_text.public_header)
            src_fh.write(wrap_text.impl_header.format(hdr_name=hdr_path.name))
            src_fh.write('''
#include "uattributes.pb.h"
#include "umessage.pb.h"
#include "upayload.pb.h"
#include "uri.pb.h"
#include "ustatus.pb.h"
#include "uuid.pb.h"
                         
''')
            # write the list of forward struct declarations to the header 
            for file in self.parse_tree:
                for obj in file:
                    if obj.type == 'message':
                        hdr_fh.write(f'struct {obj.name};\n')
            hdr_fh.write('\n')

            # write the enums before and structs after for the stuff in each proto file
            for file in self.parse_tree:
                for obj in file:
                    if obj.type == 'enum':
                        hdr_fh.write(self.make_enum(obj) + '\n')
                for obj in file:
                    if obj.type == 'message':
                        hdr_fh.write(self.make_class_decl(obj) + '\n')

            # write the impl struct definitions
            for file in self.parse_tree:
                file.reverse()
                for obj in file:
                    if obj.type == 'message':
                        src_fh.write(self.make_class_impl(obj) + '\n')
        finally:
            hdr_fh.close()
            src_fh.close()

    #############################################################
    def __call__(self, proto_file):
        proto_file = Path(proto_file)
        with open(proto_file, 'r') as fh:
            data = fh.read()
        self.chunks = []
        lcntr = 0
        self.starts = []
        m = re.search(r'package\s+(\w+)\.(\w+)\s*;', data)
        if m:
            found_package = m.group(1) + '::' + m.group(2)
        # else:
        #     raise Exception('Package not found')
        # if found_package != self.proto_nspace:
        #     raise Exception(f'package={found_package} from {proto_file} is not expected {self.proto_nspace}')
        data = self.normalize_text(data)
        parse_tree = []
        while True:
            body = statement_matcher.match(data)
            parse_tree.append(Node(body[2], body[1], parse_message(body[3])))
            if body[1] == 'enum':
                self.enum_set.add(body[2])
            data = data[body.end():]
            if len(data) == 0:
                break
            if data[0] == '\n':
                data = data[1:]
            if len(data) == 0:
                break
        self.parse_tree.append(parse_tree)

    #############################################################
    def normalize_text(self, data):
        data = re.sub(r'/\*[^*]*\*+(?:[^/*][^*]*\*+)*/',
                      '', data, re.MULTILINE)
        data = re.sub(r'\s*//[^\n]*', '', data)
        data = re.sub(r'(syntax|package|option|import)\s+[^\n]*', '', data)
        data = re.sub(r'(\s+\w+\s*=\s*\d+)\s*\[[^]]*];', r'\1;', data)
        data = re.sub(r'(\s+\n)+', '\n', data, re.MULTILINE)
        if data[0] == '\n':
            data = data[1:]
        return data

    #############################################################
    def make_enum(self, rec):
        results = f'enum {rec.name} {{\n'
        for row in rec.body:
            results += f'    {row.name} = {row.value},\n'
        results = results[:-2] + '\n'
        results += '};\n'
        return results

    #############################################################
    def make_class_decl(self, rec):
        results = wrap_text.public_class_header.format(cls_name=rec.name)
        # self.provided_types.add(rec.name)
        for row in rec.body:
            if isinstance(row, MemberRow):
                mapped_type, native_type = self.get_mapped_type(row.type, scoped=False)
                if mapped_type is None:
                    continue
                if row.attr is None:
                    results += wrap_text.public_required_member.format(
                        cls_name=rec.name, member_name=row.name, arg_type=mapped_type)
                elif row.attr == 'optional':
                    results += wrap_text.public_optional_member.format(
                        cls_name=rec.name, member_name=row.name, arg_type=mapped_type)
                # elif row.attr == 'repeated':
                #     results += wrap_text.public_repeated_member.format(
                #         cls_name=rec.name, member_name=row.name, arg_type=mapped_type)
            elif isinstance(row, Node) and row.type == 'oneof':
                var_types = [x.type for x in row.body]
                if len(var_types) == len(set(var_types)):
                    var_types = [self.get_mapped_type(x, scoped=False)[0] for x in var_types]
                    var_types = f'std::variant<{",".join(var_types)}>'
                    results += wrap_text.public_required_member.format(
                        cls_name=rec.name, member_name=row.name, arg_type=var_types)
                else:
                    print(f'skipping problematic oneof in {rec.name} in headers')
            else:
                raise Exception(f'unhandled {row}')
        results += '};\n'
        return results

    #############################################################
    def make_class_impl(self, rec):
        results = wrap_text.impl_private_class_header.format(
            cls_name=rec.name, nspace=self.proto_nspace)
        for row in rec.body:
            if isinstance(row, MemberRow):
                mapped_type, native_type = self.get_mapped_type(row.type, scoped=False)
                if mapped_type is None:
                    continue
                if row.attr is None:
                    if mapped_type in self.enum_set:
                        results += wrap_text.impl_required_member_enum.format(
                            cls_name=rec.name, member_name=row.name, arg_type=mapped_type, nspace=self.proto_nspace)
                    else:
                        if native_type:
                            results += wrap_text.impl_required_native_member.format(
                                cls_name=rec.name, member_name=row.name, arg_type=mapped_type, nspace=self.proto_nspace)
                        else:
                            results += wrap_text.impl_required_nonnative_member.format(
                                cls_name=rec.name, member_name=row.name, arg_type=mapped_type, nspace=self.proto_nspace)
                elif row.attr == 'optional':
                    if mapped_type in self.enum_set:
                        results += wrap_text.impl_optional_member_enum.format(
                            cls_name=rec.name, member_name=row.name, arg_type=mapped_type, nspace=self.proto_nspace)
                    else:
                        results += wrap_text.impl_optional_member.format(
                            cls_name=rec.name, member_name=row.name, arg_type=mapped_type, nspace=self.proto_nspace)
                # elif row.attr == 'repeated':
                #     if mapped_type in self.enum_set:
                #         results += wrap_text.impl_repeated_member_eunm.format(
                #             cls_name=rec.name, member_name=row.name, arg_type=mapped_type, nspace=self.proto_nspace)
                #     else:
                #         results += wrap_text.impl_repeated_member.format(
                #             cls_name=rec.name, member_name=row.name, arg_type=mapped_type, nspace=self.proto_nspace)
            elif isinstance(row, Node) and row.type == 'oneof':
                var_types = [x.type for x in row.body]
                if len(var_types) == len(set(var_types)):
                    var_types = { x.name:self.get_mapped_type(x.type, scoped=False) for x in row.body }
                    print(var_types)
                    if len(var_types) == len(set(var_types)):
                        results += wrap_text.impl_oneof_member(
                            cls_name=rec.name, member_name=row.name, arg_type=var_types, nspace=self.proto_nspace)
                else:
                    print(f'skipping problematic oneof in {rec.name} in source')
            results += '\n'
        return results

    #############################################################
    def get_mapped_type(self, t, scoped=True):
        if t not in type_map:
            return t, False
        t = type_map[t]
        if t is None:
            return None, False
        if scoped is True:
            t = f'{self.proto_nspace}::{t}'
        return t, True
