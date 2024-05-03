#################################################################################################
public_header = '''#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <optional>
#include <variant>
#include <vector>

'''

#################################################################################################
impl_header = '''#include "{hdr_name}"
#include <iostream>

template <typename T, typename FN, class... VAR_TYPES>
void generic_set_check(const std::variant<VAR_TYPES...>& v, FN&& fn)
{{
    if constexpr (std::is_same<T, std::basic_string<uint8_t>>::value) {{
        if (!std::holds_alternative<std::basic_string<uint8_t>>(v)) return;
        auto value = std::get<std::basic_string<uint8_t>>(v);
        const char* ptr = (const char*)value.data();
        std::string sv(ptr, ptr+value.size());
        fn(sv);
    }}
    else {{
        if (!std::holds_alternative<T>(v)) return;
        fn(std::get<T>(v));
    }}
}}

template <typename T, typename V>
T generic_get_convert(const V& value)
{{
    if constexpr (std::is_same<V, std::string>::value && std::is_same<T, std::basic_string<uint8_t>>::value) {{
        auto ptr = (const uint8_t*)value.data();
        return std::basic_string<uint8_t>(ptr, ptr + value.size());
    }}
    else {{
        return value;
    }}
}}


template <typename T>
static void trace_outer(const char * func, int lineno, const void* _this, const std::shared_ptr<T>& pImpl)
{{
    using namespace std;
    cout << func << " line=" << lineno << " this=" << _this << " pImpl=" << pImpl.get() << '/' << pImpl.use_count() << endl;
}}
#define TRACE_OUTER(_this, pImpl) trace_outer(__PRETTY_FUNCTION__, __LINE__, _this, pImpl);


static void trace_impl(const char * func, int lineno, const void* _this, const void* ptr)
{{
    using namespace std;
    cout << func << " lineno=" << lineno << " this=" << _this << " ptr=" << ptr << endl;
}}
#define TRACE_IMPL(_this, ptr) trace_impl(__PRETTY_FUNCTION__, __LINE__, _this, ptr);

'''

#################################################################################################
public_class_header = '''\
struct {cls_name} {{
    struct Impl;
    std::shared_ptr<Impl> pImpl;

    {cls_name}();
    ~{cls_name}();
    std::string serialize() const;
    void deserialize(const std::string&);
    std::string to_string() const;
'''

#################################################################################################
public_required_member = '''
    {cls_name}& {member_name}(const {arg_type}&);
    {arg_type} {member_name}() const;
'''

#################################################################################################
public_optional_member = '''
    {cls_name}& {member_name}(const {arg_type}&);
    std::optional<{arg_type}> {member_name}() const;
'''

#################################################################################################
public_repeated_member = '''
    {cls_name}& {member_name}(const std::vector<{arg_type}>&);
    std::vector<{arg_type}> {member_name}() const;
'''

#################################################################################################
impl_private_class_header = '''\
struct {cls_name}::Impl {{
    {nspace}::{cls_name}* ptr;

    inline Impl()
    {{
        ptr = new {nspace}::{cls_name}();
        TRACE_IMPL(this, ptr)
    }}

    inline ~Impl()
    {{
        TRACE_IMPL(this, ptr)
        delete ptr;
        TRACE_IMPL(this, ptr)
    }}
}};

{cls_name}::{cls_name}() : pImpl(new Impl)
{{
    TRACE_OUTER(this, pImpl)
}}

{cls_name}::~{cls_name}()
{{
    TRACE_OUTER(this, pImpl)
}}

std::string {cls_name}::serialize() const
{{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}}

void {cls_name}::deserialize(const std::string& data)
{{
    pImpl->ptr->ParseFromString(data);
}}

std::string {cls_name}::to_string() const
{{
    return pImpl->ptr->DebugString();
}}
'''


#################################################################################################
impl_required_native_member = '''
{cls_name}& {cls_name}::{member_name}(const {arg_type}& arg)
{{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_{member_name}(arg);
    return *this;
}}

{arg_type} {cls_name}::{member_name}() const
{{
    TRACE_OUTER(this, pImpl)
    return pImpl->ptr->{member_name}();
}}
'''

#################################################################################################
impl_required_nonnative_member = '''
{cls_name}& {cls_name}::{member_name}(const {arg_type}& arg)
{{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_allocated_{member_name}(arg.pImpl->ptr);
    return *this;
}}

{arg_type} {cls_name}::{member_name}() const
{{
    TRACE_OUTER(this, pImpl)
    {arg_type} ret;
    *(ret.pImpl->ptr) = pImpl->ptr->{member_name}();
    return ret;
}}
'''

#################################################################################################
impl_required_member_enum = '''
{cls_name}& {cls_name}::{member_name}(const {arg_type}& arg)
{{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_{member_name}(static_cast<{nspace}::{arg_type}>(arg));
    return *this;
}}

{arg_type} {cls_name}::{member_name}() const
{{
    return static_cast<{arg_type}>(pImpl->ptr->{member_name}());
}}
'''

#################################################################################################
impl_optional_member = '''
{cls_name}& {cls_name}::{member_name}(const {arg_type}& arg)
{{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_{member_name}(arg);
    return *this;
}}

std::optional<{arg_type}> {cls_name}::{member_name}() const
{{
    TRACE_OUTER(this, pImpl)
    if (!pImpl->ptr->has_{member_name}()) return std::optional<{arg_type}>();
    return pImpl->ptr->{member_name}();
}}
'''

#################################################################################################
impl_optional_member_enum = '''
{cls_name}& {cls_name}::{member_name}(const {arg_type}& arg)
{{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_{member_name}(static_cast<{nspace}::{arg_type}>(arg));
    return *this;
}}

std::optional<{arg_type}> {cls_name}::{member_name}() const
{{
    TRACE_OUTER(this, pImpl)
    if (!pImpl->ptr->has_{member_name}()) return std::optional<{arg_type}>();
    return static_cast<{arg_type}>(pImpl->ptr->{member_name}());
}}
'''

#################################################################################################
impl_repeated_member = '''
{cls_name}& {cls_name}::{member_name}(const std::vector<{arg_type}>& arg)
{{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->clear_{member_name}();
    for (const auto& n : arg) {{
        auto p = pImpl->ptr->add_{member_name}();
        *p = n;
    }}
    return *this;
}}

std::vector<{arg_type}> {cls_name}::{member_name}() const
{{
    TRACE_OUTER(this, pImpl)
    std::vector<{arg_type}> ret;
    size_t len = pImpl->ptr->{member_name}_size();
    ret.reserve(len);
    for (size_t i = 0; i < len; i++) ret.push_back(pImpl->ptr->{member_name}(i));
    return ret;
}}
'''

#################################################################################################
impl_repeated_member_enum = '''
{cls_name}& {cls_name}::{member_name}(const std::vector<{arg_type}>& arg)
{{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->clear_{member_name}();
    for (const auto& n : arg) {{
        auto p = pImpl->ptr->add_{member_name}();
        *p = static_cast<{nspace}::{arg_type}>(n);
    }}
    return *this;
}}

std::vector<{arg_type}> {cls_name}::{member_name}() const
{{
    TRACE_OUTER(this, pImpl)
    std::vector<{arg_type}> ret;
    size_t len = pImpl->ptr->{member_name}_size();
    ret.reserve(len);
    for (size_t i = 0; i < len; i++) ret.push_back(static_cast<{arg_type}>(pImpl->ptr->{member_name}(i)));
    return ret;
}}
'''

#################################################################################################
def impl_oneof_member(cls_name, member_name, arg_type, nspace):
    print(f'impl_oneof_member arg_type={arg_type}')
    v_list = ", ".join([ x[0] for x in arg_type.values() ])
    set_body = ''
    get_body = ''
    def_value = list(arg_type.keys())[0]
    for name,ftype in arg_type.items():
        set_body += f'    generic_set_check<{ftype[0]}>(arg, [&](auto larg){{ pImpl->ptr->set_{name}(larg); }});\n'
        get_body += f'    if (pImpl->ptr->has_{name}()) return generic_get_convert<{ftype[0]}>(pImpl->ptr->{name}());\n'
    get_body += f'    return pImpl->ptr->{def_value}();\n'
    results = f'''
{cls_name}& {cls_name}::{member_name}(const std::variant<{v_list}>& arg)
{{
    TRACE_OUTER(this, pImpl)
{set_body}
    return *this;
}}

std::variant<{v_list}> {cls_name}::{member_name}() const
{{
    TRACE_OUTER(this, pImpl)
{get_body}
}}
'''
    return results

#                     var_types = f'std::variant<{",".join(var_types)}>'
