/* ====================================================================
 * Copyright (c) 1995-1999 The Apache Group.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * 4. The names "Apache Server" and "Apache Group" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache"
 *    nor may "Apache" appear in their names without prior written
 *    permission of the Apache Group.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * THIS SOFTWARE IS PROVIDED BY THE APACHE GROUP ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE APACHE GROUP OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Group and was originally based
 * on public domain software written at the National Center for
 * Supercomputing Applications, University of Illinois, Urbana-Champaign.
 * For more information on the Apache Group and the Apache HTTP server
 * project, please see <http://www.apache.org/>.
 *
 */

#ifndef _BASE64_H_
#define _BASE64_H_

#ifndef __cplusplus
#include <stdlib.h>
#else /* __cplusplus */
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <string>
#endif /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif

int Base64encode_len(int len);
int Base64encode(char* coded_dst, const char* plain_src, int len_plain_src);

size_t Base64decode_len(const char* coded_src);
int Base64decode(char* plain_dst, const char* coded_src);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace cloudevents {
class base64 {
 public:
  static std::string encode(const std::string& t_str) {
    size_t sz_size = Base64encode_len(t_str.length());

    char* pc_dst = new char[sz_size];
    if (nullptr == pc_dst) {
      return "";
    }

    size_t sz_str_length = t_str.length();
    if (sz_str_length > INT_MAX) {
      return "";
    }

    int i_val = Base64encode(pc_dst, t_str.c_str(),
                             (int)sz_str_length); /* TODO: safe convert */
    if ((((size_t)i_val) != (sz_size - 4)) && (((size_t)i_val) != sz_size)) {
      return "";
    }

    std::string ret_string(pc_dst);
    delete[] pc_dst;
    pc_dst = nullptr;
    return ret_string;
  }

  static std::string decode(const std::string& t_str) {
    size_t sz_size = Base64decode_len(t_str.c_str());

    char* pc_str = (char*)new char[sz_size];
    if (nullptr == pc_str) {
      return "";
    }

    Base64decode(pc_str, t_str.c_str());
    std::string t_ret_str(pc_str);
    delete pc_str;
    pc_str = nullptr;
    return t_ret_str;
  }

 private:
  base64() = default;
};
}  // namespace cloudevents
#endif /* __cplusplus */

#endif  //_BASE64_H_