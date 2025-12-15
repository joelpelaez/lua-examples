#ifndef EXAMPLES_H
#define EXAMPLES_H

/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2025 Joel Pelaez Jorge
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

/* Define macro for unused parameters */
#if defined(__GNUC__) || defined(__clang__)
#define __UNUSED __attribute__((unused))
#else
#define __UNUSED
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Run basic REPL */
void run_lua_interpreter(void);

/* Create a coroutine in C and resume it from Lua. */
void create_coroutine_in_c_and_call_it_from_lua(void);

/* Create a coroutine in Lua and resume it from C. */
void create_lua_coroutine_and_manage_in_c(void);

/* Create a coroutine and start it in C with some Lua code inside. */
void direct_call_to_coroutine(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* EXAMPLES_H */
