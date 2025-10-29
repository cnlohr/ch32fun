// clang-format off
// flag.h -- v1.7.1 -- command-line flag parsing
//
//   Inspired by Go's flag module: https://pkg.go.dev/flag
//
// # Macros API
//
// - FLAG_LIST_INIT_CAP - initial capacity of the Flag_List and Flag_List_Mut Dynamic Arrays.
// - FLAGS_CAP - how many flags you can define within a single context.
// - FLAG_PUSH_DASH_DASH_BACK - make flag_parse() retain "--" in the rest args
//   (available via flag_rest_argc() and flag_rest_argv()). Useful when you need
//   to know whether flag_parse() has stopped due to encountering "--" or due to
//   encountering a non-flag. Ideally this should've been a default behavior,
//   but it breaks backward compatibility. Hence it's a feature macro.
//
//   TODO: make FLAG_PUSH_DASH_DASH_BACK a default behavior on a major version upgrade.
//   Or maybe even better just expose some sort of a flag that tells the user whether
//   the dash-dash was encountered or not.
//
// # Ignoring Flags
//
//   Flag.h implements an experimental syntax for ignoring flags. Consider the following command line:
//
//   ```console
//   $ ./command -arg1 value1 -arg2 -arg3 value3
//   ```
//
//   It provides three arguments `-arg1 value1`, `-arg2`, and `-arg3 value3`. By putting a forward slash `/`
//   after the dash `-` in front of the argument you tell flag.h to parse the argument as usual, check the
//   syntax check the type, but treat it as it was never provided.
//
//   ```console
//   $ ./command -/arg1 value1 -arg2 -arg3 value3
//   ```
//
//   In the above example only `-arg2` and `-arg3 value3` are provided, while `-/arg1 value1` is parsed, but
//   ignored.
//
//   This enables you to "comment out" certain arguments so you can reenable them later as you rerun the same
//   command over and over again in the terminal by pressing Up and then Enter.
#ifndef FLAG_H_
#define FLAG_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <inttypes.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <float.h>

#ifndef FLAGS_CAP
#define FLAGS_CAP 256
#endif // FLAGS_CAP

#ifndef FLAG_LIST_INIT_CAP
#define FLAG_LIST_INIT_CAP 8
#endif // FLAG_LIST_INIT_CAP

// Works with both Flag_List and Flag_List_Mut
#define flag_list_append(type, list, item)                                                         \
    do {                                                                                           \
        if ((list)->count >= (list)->capacity) {                                                   \
            size_t new_capacity = (list)->capacity == 0 ? FLAG_LIST_INIT_CAP : (list)->capacity*2; \
            (list)->items = (type*)realloc((list)->items, new_capacity*sizeof(*(list)->items));    \
            (list)->capacity = new_capacity;                                                       \
        }                                                                                          \
                                                                                                   \
        (list)->items[(list)->count++] = item;                                                     \
    } while(0)

typedef struct {
    const char **items;
    size_t count;
    size_t capacity;
} Flag_List;

// The only reason Flag_List_Mut exists is to enable recursive usage of flag[_c]_parse(..) in a backward compatible manner.
// That is using flag[_c]_parse(..) on Flag_List_Mut-s acquired from other flag[_c]_parse(..) calls, since argv must be mutable
// to enable -flag=value syntax.
// TODO: It was a mistake to make items const in Flag_List in the first place. In the next major release get rid of Flag_List_Mut and make items mutable in Flag_List
typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} Flag_List_Mut;

/// API that operate on implicit global flag context

char *flag_name(void *val);

bool *flag_bool(const char *name, bool def, const char *desc);
void flag_bool_var(bool *var, const char *name, bool def, const char *desc);

float *flag_float(const char *name, float def, const char *desc);
void flag_float_var(float *var, const char *name, float def, const char *desc);

double *flag_double(const char *name, double def, const char *desc);
void flag_double_var(double *var, const char *name, double def, const char *desc);

uint64_t *flag_uint64(const char *name, uint64_t def, const char *desc);
void flag_uint64_var(uint64_t *var, const char *name, uint64_t def, const char *desc);

size_t *flag_size(const char *name, uint64_t def, const char *desc);
void flag_size_var(size_t *var, const char *name, uint64_t def, const char *desc);

char **flag_str(const char *name, const char *def, const char *desc);
void flag_str_var(char **var, const char *name, const char *def, const char *desc);

Flag_List *flag_list(const char *name, const char *desc);
void flag_list_var(Flag_List *var, const char *name, const char *desc);

Flag_List_Mut *flag_list_mut(const char *name, const char *desc);
void flag_list_mut_var(Flag_List_Mut *var, const char *name, const char *desc);

bool flag_parse(int argc, char **argv);
int flag_rest_argc(void);
char **flag_rest_argv(void);
const char *flag_program_name(void);
void flag_print_error(FILE *stream);
void flag_print_options(FILE *stream);

/// API that operate on a custom opaque flag context.

// Allocate a new opaque flag context.
//
// Setting `program_name` is needed so `flag_c_parse(c, argc, argv)` does not try to consume
// extra argument from `argv` as it usually does. If that's exactly what you want just pass
// `NULL` as the `program_name` then.
//
// If specific `program_name` does not make sense in your case, you can just set it to whatever,
// since internally it's not used for anything except consuming an extra argument at the beginning
// of flag[_c]_parse(..) and giving that argument back to the user via flag[_c]_program_name(..).
// If you are designing a subcommand system where each subcommand has its own context you can set
// `program_name` to the name of the corresponding subcommand.
void *flag_c_new(const char *program_name);
void flag_c_free(void *c);
char *flag_c_name(void *c, void *val);

bool *flag_c_bool(void *c, const char *name, bool def, const char *desc);
void flag_c_bool_var(void *c, bool *var, const char *name, bool def, const char *desc);

float *flag_c_float(void *c, const char *name, float def, const char *desc);
void flag_c_float_var(void *c, float *var, const char *name, float def, const char *desc);

double *flag_c_double(void *c, const char *name, double def, const char *desc);
void flag_c_double_var(void *c, double *var, const char *name, double def, const char *desc);

uint64_t *flag_c_uint64(void *c, const char *name, uint64_t def, const char *desc);
void flag_c_uint64_var(void *c, uint64_t *var, const char *name, uint64_t def, const char *desc);

size_t *flag_c_size(void *c, const char *name, uint64_t def, const char *desc);
void flag_c_size_var(void *c, size_t *var, const char *name, uint64_t def, const char *desc);

char **flag_c_str(void *c, const char *name, const char *def, const char *desc);
void flag_c_str_var(void *c, char **var, const char *name, const char *def, const char *desc);

Flag_List *flag_c_list(void *c, const char *name, const char *desc);
void flag_c_list_var(void *c, Flag_List *var, const char *name, const char *desc);

Flag_List_Mut *flag_c_list_mut(void *c, const char *name, const char *desc);
void flag_c_list_mut_var(void *c, Flag_List_Mut *var, const char *name, const char *desc);

bool flag_c_parse(void *c, int argc, char **argv);
int flag_c_rest_argc(void *c);
char **flag_c_rest_argv(void *c);
const char *flag_c_program_name(void *c);
void flag_c_print_error(void *c, FILE *stream);
void flag_c_print_options(void *c, FILE *stream);

#endif // FLAG_H_

//////////////////////////////

#ifdef FLAG_IMPLEMENTATION

typedef enum {
    FLAG_BOOL = 0,
    FLAG_UINT64,
    FLAG_DOUBLE,
    FLAG_FLOAT,
    FLAG_SIZE,
    FLAG_STR,
    FLAG_LIST,
    FLAG_LIST_MUT,
    COUNT_FLAG_TYPES,
} Flag_Type;

static_assert(COUNT_FLAG_TYPES == 8, "Exhaustive Flag_Value definition");
typedef union {
    char *as_str;
    uint64_t as_uint64;
    double as_double;
    float as_float;
    bool as_bool;
    size_t as_size;
    Flag_List as_list;
    Flag_List_Mut as_list_mut;
} Flag_Value;

typedef enum {
    FLAG_NO_ERROR = 0,
    FLAG_ERROR_UNKNOWN,
    FLAG_ERROR_NO_VALUE,
    FLAG_ERROR_INVALID_NUMBER,
    FLAG_ERROR_INTEGER_OVERFLOW,
    FLAG_ERROR_FLOAT_OVERFLOW,
    FLAG_ERROR_DOUBLE_OVERFLOW,
    FLAG_ERROR_INVALID_SIZE_SUFFIX,
    COUNT_FLAG_ERRORS,
} Flag_Error;

typedef struct {
    Flag_Type type;
    char *name;
    char *desc;

    Flag_Value val;
    void *ref;

    Flag_Value def;
} Flag;

typedef struct {
    Flag flags[FLAGS_CAP];
    size_t flags_count;

    Flag_Error flag_error;
    char *flag_error_name;

    const char *program_name;

    int rest_argc;
    char **rest_argv;
} Flag_Context;

// Forward declaration of private functions
static Flag *flag__new_flag(Flag_Context *c, Flag_Type type, const char *name, const char *desc);
static void *flag__get_ref(Flag *flag);
static bool flag__size_calculate_multiplier(char* endptr, unsigned long long int* result);

static Flag_Context flag_global_context;

void *flag_c_new(const char *program_name)
{
    Flag_Context *fc = (Flag_Context *)malloc(sizeof(*fc));
    memset(fc, 0, sizeof(*fc));
    fc->program_name = program_name;
    return fc;
}

void flag_c_free(void *c)
{
    free(c);
}

static Flag *flag__new_flag(Flag_Context *c, Flag_Type type, const char *name, const char *desc)
{
    assert(c->flags_count < FLAGS_CAP);
    Flag *flag = &c->flags[c->flags_count++];
    memset(flag, 0, sizeof(*flag));
    flag->type = type;
    // NOTE: I won't touch them I promise Kappa
    flag->name = (char*) name;
    flag->desc = (char*) desc;
    return flag;
}

static void *flag__get_ref(Flag *flag)
{
    if (flag->ref) return flag->ref;
    return &flag->val;
}

char *flag_name(void *val)
{
    return flag_c_name(&flag_global_context, val);
}

char *flag_c_name(void *c, void *val)
{
    Flag_Context *fc = (Flag_Context*)c;

    for (size_t i = 0; i < fc->flags_count; ++i) {
        Flag *flag = &fc->flags[i];
        if (flag__get_ref(flag) == val) {
            return flag->name;
        }
    }

    return NULL;
}

bool *flag_c_bool(void *c, const char *name, bool def, const char *desc)
{
    Flag *flag = flag__new_flag((Flag_Context*)c, FLAG_BOOL, name, desc);
    flag->def.as_bool = def;
    flag->val.as_bool = def;
    return &flag->val.as_bool;
}

void flag_c_bool_var(void *c, bool *var, const char *name, bool def, const char *desc)
{
    Flag *flag = flag__new_flag((Flag_Context*)c, FLAG_BOOL, name, desc);
    flag->def.as_bool = def;
    flag->ref = var;
    *var = def;
}

bool *flag_bool(const char *name, bool def, const char *desc)
{
    return flag_c_bool(&flag_global_context, name, def, desc);
}

void flag_bool_var(bool *var, const char *name, bool def, const char *desc)
{
    flag_c_bool_var(&flag_global_context, var, name, def, desc);
}

float *flag_c_float(void *c, const char *name, float def, const char *desc)
{
    Flag *flag = flag__new_flag((Flag_Context*)c, FLAG_FLOAT, name, desc);
    flag->def.as_float = def;
    flag->val.as_float = def;
    return &flag->val.as_float;
}

void flag_c_float_var(void *c, float *var, const char *name, float def, const char *desc)
{
    Flag *flag = flag__new_flag((Flag_Context*)c, FLAG_FLOAT, name, desc);
    flag->def.as_float = def;
    flag->ref = var;
    *var = def;
}

double *flag_c_double(void *c, const char *name, double def, const char *desc)
{
    Flag *flag = flag__new_flag((Flag_Context*)c, FLAG_DOUBLE, name, desc);
    flag->def.as_double = def;
    flag->val.as_double = def;
    return &flag->val.as_double;
}

void flag_c_double_var(void *c, double *var, const char *name, double def, const char *desc)
{
    Flag *flag = flag__new_flag((Flag_Context*)c, FLAG_DOUBLE, name, desc);
    flag->def.as_double = def;
    flag->ref = var;
    *var = def;
}

float *flag_float(const char *name, float def, const char *desc)
{
    return flag_c_float(&flag_global_context, name, def, desc);
}

void flag_float_var(float *var, const char *name, float def, const char *desc)
{
    flag_c_float_var(&flag_global_context, var, name, def, desc);
}

double *flag_double(const char *name, double def, const char *desc)
{
    return flag_c_double(&flag_global_context, name, def, desc);
}

void flag_double_var(double *var, const char *name, double def, const char *desc)
{
    flag_c_double_var(&flag_global_context, var, name, def, desc);
}

uint64_t *flag_c_uint64(void *c, const char *name, uint64_t def, const char *desc)
{
    Flag *flag = flag__new_flag((Flag_Context*)c, FLAG_UINT64, name, desc);
    flag->val.as_uint64 = def;
    flag->def.as_uint64 = def;
    return &flag->val.as_uint64;
}

void flag_c_uint64_var(void *c, uint64_t *var, const char *name, uint64_t def, const char *desc)
{
    Flag *flag = flag__new_flag((Flag_Context*)c, FLAG_UINT64, name, desc);
    flag->def.as_uint64 = def;
    flag->ref = var;
    *var = def;
}

uint64_t *flag_uint64(const char *name, uint64_t def, const char *desc)
{
    return flag_c_uint64(&flag_global_context, name, def, desc);
}

void flag_uint64_var(uint64_t *var, const char *name, uint64_t def, const char *desc)
{
    flag_c_uint64_var(&flag_global_context, var, name, def, desc);
}

size_t *flag_c_size(void *c, const char *name, uint64_t def, const char *desc)
{
    Flag *flag = flag__new_flag((Flag_Context*)c, FLAG_SIZE, name, desc);
    flag->val.as_size = def;
    flag->def.as_size = def;
    return &flag->val.as_size;
}

void flag_c_size_var(void *c, size_t *var, const char *name, uint64_t def, const char *desc)
{
    Flag *flag = flag__new_flag((Flag_Context*)c, FLAG_SIZE, name, desc);
    flag->ref = var;
    flag->def.as_size = def;
    *var = def;
}

size_t *flag_size(const char *name, uint64_t def, const char *desc)
{
    return flag_c_size(&flag_global_context, name, def, desc);
}

void flag_size_var(size_t *var, const char *name, uint64_t def, const char *desc)
{
    flag_c_size_var(&flag_global_context, var, name, def, desc);
}

char **flag_c_str(void *c, const char *name, const char *def, const char *desc)
{
    Flag *flag = flag__new_flag((Flag_Context*)c, FLAG_STR, name, desc);
    flag->val.as_str = (char*) def;
    flag->def.as_str = (char*) def;
    return &flag->val.as_str;
}

void flag_c_str_var(void *c, char **var, const char *name, const char *def, const char *desc)
{
    Flag *flag = flag__new_flag((Flag_Context*)c, FLAG_STR, name, desc);
    flag->ref = var;
    flag->def.as_str = (char*) def;
    *var = (char*) def;
}

char **flag_str(const char *name, const char *def, const char *desc)
{
    return flag_c_str(&flag_global_context, name, def, desc);
}

void flag_str_var(char **var, const char *name, const char *def, const char *desc)
{
    flag_c_str_var(&flag_global_context, var, name, def, desc);
}

Flag_List *flag_c_list(void *c, const char *name, const char *desc)
{
    Flag *flag = flag__new_flag((Flag_Context*)c, FLAG_LIST, name, desc);
    return &flag->val.as_list;
}

void flag_c_list_var(void *c, Flag_List *var, const char *name, const char *desc)
{
    Flag *flag = flag__new_flag((Flag_Context*)c, FLAG_LIST, name, desc);
    flag->ref = var;
}

Flag_List_Mut *flag_c_list_mut(void *c, const char *name, const char *desc)
{
    Flag *flag = flag__new_flag((Flag_Context*)c, FLAG_LIST_MUT, name, desc);
    return &flag->val.as_list_mut;
}

void flag_c_list_mut_var(void *c, Flag_List_Mut *var, const char *name, const char *desc)
{
    Flag *flag = flag__new_flag((Flag_Context*)c, FLAG_LIST_MUT, name, desc);
    flag->ref = var;
}

Flag_List *flag_list(const char *name, const char *desc)
{
    return flag_c_list(&flag_global_context, name, desc);
}

void flag_list_var(Flag_List *var, const char *name, const char *desc)
{
    flag_c_list_var(&flag_global_context, var, name, desc);
}

Flag_List_Mut *flag_list_mut(const char *name, const char *desc)
{
    return flag_c_list_mut(&flag_global_context, name, desc);
}

void flag_list_mut_var(Flag_List_Mut *var, const char *name, const char *desc)
{
    flag_c_list_mut_var(&flag_global_context, var, name, desc);
}

static char *flag_shift_args(int *argc, char ***argv)
{
    assert(*argc > 0);
    char *result = **argv;
    *argv += 1;
    *argc -= 1;
    return result;
}

int flag_rest_argc(void)
{
    return flag_global_context.rest_argc;
}

int flag_c_rest_argc(void *c)
{
    return ((Flag_Context*)c)->rest_argc;
}

char **flag_rest_argv(void)
{
    return flag_global_context.rest_argv;
}

char **flag_c_rest_argv(void *c)
{
    return ((Flag_Context*)c)->rest_argv;
}

const char *flag_program_name(void)
{
    return flag_global_context.program_name;
}

const char *flag_c_program_name(void *c)
{
    return ((Flag_Context*)c)->program_name;
}

void flag_c_set_program_name(void *c, const char *program_name)
{
    ((Flag_Context *)c)->program_name = program_name;
}

static bool flag__size_calculate_multiplier(char* endptr, unsigned long long int* result)
{
    if (strcmp(endptr, "c") == 0) {
        (*result) *= 1ULL;
    } else if (strcmp(endptr, "w") == 0) {
        (*result) *= 2ULL;
    } else if (strcmp(endptr, "b") == 0) {
        (*result) *= 512ULL;
    } else if (strcmp(endptr, "kB") == 0) {
        (*result) *= 1000ULL;
    } else if (strcmp(endptr, "K") == 0 || strcmp(endptr, "KiB") == 0) {
        (*result) *= 1024ULL;
    } else if (strcmp(endptr, "MB") == 0) {
        (*result) *= 1000ULL * 1000ULL;
    } else if (strcmp(endptr, "M") == 0 || strcmp(endptr, "MiB") == 0 || strcmp(endptr, "xM") == 0) {
        (*result) *= 1024ULL * 1024ULL;
    } else if (strcmp(endptr, "GB") == 0) {
        (*result) *= 1000ULL * 1000ULL * 1000ULL;
    } else if (strcmp(endptr, "G") == 0 || strcmp(endptr, "GiB") == 0) {
        (*result) *= 1024ULL * 1024ULL * 1024ULL;
    } else if (strcmp(endptr, "TB") == 0) {
        (*result) *= 1000ULL * 1000ULL * 1000ULL * 1000ULL;
    } else if (strcmp(endptr, "T") == 0 || strcmp(endptr, "TiB") == 0) {
        (*result) *= 1024ULL * 1024ULL * 1024ULL * 1024ULL;
    } else if (strcmp(endptr, "PB") == 0) {
        (*result) *= 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL;
    } else if (strcmp(endptr, "P") == 0 || strcmp(endptr, "PiB") == 0) {
        (*result) *= 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL;
    } else if (strcmp(endptr, "EB") == 0) {
        (*result) *= 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL;
    } else if (strcmp(endptr, "E") == 0 || strcmp(endptr, "EiB") == 0) {
        (*result) *= 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL;
    } else if (strcmp(endptr, "ZB") == 0) {
        (*result) *= 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL;
    } else if (strcmp(endptr, "Z") == 0 || strcmp(endptr, "ZiB") == 0) {
        (*result) *= 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL;
    } else if (strcmp(endptr, "YB") == 0) {
        (*result) *= 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL;
    } else if (strcmp(endptr, "Y") == 0 || strcmp(endptr, "YiB") == 0) {
        (*result) *= 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL;
    } else if (strcmp(endptr, "") != 0) {
        return false;
    }
    return true;
 }

bool flag_c_parse(void *c, int argc, char **argv)
{
    Flag_Context *fc = (Flag_Context*)c;

    if (fc->program_name == NULL) {
        fc->program_name = flag_shift_args(&argc, &argv);
    }

    while (argc > 0) {
        char *flag = flag_shift_args(&argc, &argv);

        if (*flag != '-') {
            // NOTE: pushing flag back into args
            fc->rest_argc = argc + 1;
            fc->rest_argv = argv - 1;
            return true;
        }

        if (strcmp(flag, "--") == 0) {
#ifdef FLAG_PUSH_DASH_DASH_BACK
            // NOTE: pushing dash dash back into args as requested by the user
            fc->rest_argc = argc + 1;
            fc->rest_argv = argv - 1;
#else
            // NOTE: not pushing dash dash back into args for backward compatibility
            fc->rest_argc = argc;
            fc->rest_argv = argv;
#endif // FLAG_PUSH_DASH_DASH_BACK
            return true;
        }

        // NOTE: remove the dash
        flag += 1;

        bool ignore = false;
        if (*flag == '/') {
            ignore = true;
            flag += 1;
        }

        char *equals = strchr(flag, '=');
        if (equals != NULL) {
            // trim off the '=' and the value from `flag`,
            *equals = '\0';
            // and make `equals` be a pointer to just the value
            equals += 1;
        }

        bool found = false;
        for (size_t i = 0; i < fc->flags_count; ++i) {
            if (strcmp(fc->flags[i].name, flag) == 0) {
                static_assert(COUNT_FLAG_TYPES == 8, "Exhaustive flag type parsing");
                switch (fc->flags[i].type) {
                case FLAG_LIST: {
                    char *arg;
                    if (equals == NULL) {
                        if (argc == 0) {
                            fc->flag_error = FLAG_ERROR_NO_VALUE;
                            fc->flag_error_name = flag;
                            return false;
                        }
                        arg = flag_shift_args(&argc, &argv);
                    } else {
                        arg = equals;
                    }

                    if (!ignore) {
                        Flag_List *list = (Flag_List*)flag__get_ref(&fc->flags[i]);
                        flag_list_append(const char *, list, arg);
                    }
                }
                break;

                case FLAG_LIST_MUT: {
                    char *arg;
                    if (equals == NULL) {
                        if (argc == 0) {
                            fc->flag_error = FLAG_ERROR_NO_VALUE;
                            fc->flag_error_name = flag;
                            return false;
                        }
                        arg = flag_shift_args(&argc, &argv);
                    } else {
                        arg = equals;
                    }

                    if (!ignore) {
                        Flag_List_Mut *list = (Flag_List_Mut*)flag__get_ref(&fc->flags[i]);
                        flag_list_append(char *, list, arg);
                    }
                }
                break;

                case FLAG_BOOL: {
                    // TODO: when the -flag= syntax is used, the boolean should probably parse values such as
                    // "true", "false", "on", "off", etc. But I'm not sure how backward compatible it is to
                    // introduce such syntax at this point...
                    if (!ignore) *(bool*)flag__get_ref(&fc->flags[i]) = true;
                }
                break;

                case FLAG_STR: {
                    char *arg;
                    if (equals == NULL) {
                        if (argc == 0) {
                            fc->flag_error = FLAG_ERROR_NO_VALUE;
                            fc->flag_error_name = flag;
                            return false;
                        }
                        arg = flag_shift_args(&argc, &argv);
                    } else {
                        arg = equals;
                    }

                    if (!ignore) *(char**)flag__get_ref(&fc->flags[i]) = arg;
                }
                break;

                case FLAG_UINT64: {
                    char *arg;
                    if (equals == NULL) {
                        if (argc == 0) {
                            fc->flag_error = FLAG_ERROR_NO_VALUE;
                            fc->flag_error_name = flag;
                            return false;
                        }
                        arg = flag_shift_args(&argc, &argv);
                    } else {
                        arg = equals;
                    }

                    static_assert(sizeof(unsigned long long int) == sizeof(uint64_t), "The original author designed this for x86_64 machine with the compiler that expects unsigned long long int and uint64_t to be the same thing, so they could use strtoull() function to parse it. Please adjust this code for your case and maybe even send the patch to upstream to make it work on a wider range of environments.");
                    char *endptr;
                    unsigned long long int result = strtoull(arg, &endptr, 10);

                    if (*endptr != '\0') {
                        fc->flag_error = FLAG_ERROR_INVALID_NUMBER;
                        fc->flag_error_name = flag;
                        return false;
                    }

                    if (result == ULLONG_MAX && errno == ERANGE) {
                        fc->flag_error = FLAG_ERROR_INTEGER_OVERFLOW;
                        fc->flag_error_name = flag;
                        return false;
                    }

                    if (!ignore) *(uint64_t*)flag__get_ref(&fc->flags[i]) = result;
                }
                break;

                case FLAG_FLOAT: {
                    char *arg;
                    if (equals == NULL) {
                        if (argc == 0) {
                            fc->flag_error = FLAG_ERROR_NO_VALUE;
                            fc->flag_error_name = flag;
                            return false;
                        }
                        arg = flag_shift_args(&argc, &argv);
                    } else {
                        arg = equals;
                    }
                    char *endptr;
                    float result = strtof(arg, &endptr);

                    if (*endptr != '\0') {
                        fc->flag_error = FLAG_ERROR_INVALID_NUMBER;
                        fc->flag_error_name = flag;
                        return false;
                    }

                    if (result == FLT_MAX && errno == ERANGE) {
                        fc->flag_error = FLAG_ERROR_FLOAT_OVERFLOW;
                        fc->flag_error_name = flag;
                        return false;
                    }

                    if (!ignore) *(float*)flag__get_ref(&fc->flags[i]) = result;
                }
                break;

                case FLAG_DOUBLE: {
                    char *arg;
                    if (equals == NULL) {
                        if (argc == 0) {
                            fc->flag_error = FLAG_ERROR_NO_VALUE;
                            fc->flag_error_name = flag;
                            return false;
                        }
                        arg = flag_shift_args(&argc, &argv);
                    } else {
                        arg = equals;
                    }
                    char *endptr;
                    double result = strtod(arg, &endptr);

                    if (*endptr != '\0') {
                        fc->flag_error = FLAG_ERROR_INVALID_NUMBER;
                        fc->flag_error_name = flag;
                        return false;
                    }

                    if (result == DBL_MAX && errno == ERANGE) {
                        fc->flag_error = FLAG_ERROR_DOUBLE_OVERFLOW;
                        fc->flag_error_name = flag;
                        return false;
                    }

                    if (!ignore) *(double*)flag__get_ref(&fc->flags[i]) = result;
                }
                break;

                case FLAG_SIZE: {
                    char *arg;
                    if (equals == NULL) {
                        if (argc == 0) {
                            fc->flag_error = FLAG_ERROR_NO_VALUE;
                            fc->flag_error_name = flag;
                            return false;
                        }
                        arg = flag_shift_args(&argc, &argv);
                    } else {
                        arg = equals;
                    }

                    static_assert(sizeof(unsigned long long int) == sizeof(size_t), "The original author designed this for x86_64 machine with the compiler that expects unsigned long long int and size_t to be the same thing, so they could use strtoull() function to parse it. Please adjust this code for your case and maybe even send the patch to upstream to make it work on a wider range of environments.");
                    char *endptr;
                    unsigned long long int result = strtoull(arg, &endptr, 10);

                    if (!flag__size_calculate_multiplier(endptr,&result)) {
                        fc->flag_error = FLAG_ERROR_INVALID_SIZE_SUFFIX;
                        fc->flag_error_name = flag;
                        return false;
                    }

                    if (result == ULLONG_MAX && errno == ERANGE) {
                        fc->flag_error = FLAG_ERROR_INTEGER_OVERFLOW;
                        fc->flag_error_name = flag;
                        return false;
                    }

                    if (!ignore) *(size_t*)flag__get_ref(&fc->flags[i]) = result;
                }
                break;

                case COUNT_FLAG_TYPES:
                default: {
                    assert(0 && "unreachable");
                    exit(69);
                }
                }

                found = true;
            }
        }

        // NOTE: BogdanTheGeek: save rest args on error so that another parser can pick them up
        if (!found) {
            fc->flag_error = FLAG_ERROR_UNKNOWN;
            fc->flag_error_name = flag;
            fc->rest_argc = argc + 1;
            fc->rest_argv = argv - 1;
            return false;
        }
    }

    fc->rest_argc = argc;
    fc->rest_argv = argv;
    return true;
}

bool flag_parse(int argc, char **argv)
{
    return flag_c_parse(&flag_global_context, argc, argv);
}

void flag_c_print_options(void *c, FILE *stream)
{
    Flag_Context *fc = (Flag_Context *)c;
    for (size_t i = 0; i < fc->flags_count; ++i) {
        Flag *flag = &fc->flags[i];

        static_assert(COUNT_FLAG_TYPES == 8, "Exhaustive flag type defaults printing");
        switch (fc->flags[i].type) {
        case FLAG_LIST_MUT:
        case FLAG_LIST:
            fprintf(stream, "    -%s <str> ... -%s <str> ...\n", flag->name, flag->name);
            fprintf(stream, "        %s\n", flag->desc);
            break;
        case FLAG_BOOL:
            fprintf(stream, "    -%s\n", flag->name);
            fprintf(stream, "        %s\n", flag->desc);
            if (flag->def.as_bool) {
                fprintf(stream, "        Default: %s\n", flag->def.as_bool ? "true" : "false");
            }
            break;
        case FLAG_UINT64:
            fprintf(stream, "    -%s <int>\n", flag->name);
            fprintf(stream, "        %s\n", flag->desc);
            fprintf(stream, "        Default: %" PRIu64 "\n", flag->def.as_uint64);
            break;
        case FLAG_FLOAT:
            fprintf(stream, "    -%s <float>\n", flag->name);
            fprintf(stream, "        %s\n", flag->desc);
            fprintf(stream, "        Default: %f\n", flag->def.as_float);
            break;
        case FLAG_DOUBLE:
            fprintf(stream, "    -%s <double>\n", flag->name);
            fprintf(stream, "        %s\n", flag->desc);
            fprintf(stream, "        Default: %lf\n", flag->def.as_double);
            break;
        case FLAG_SIZE:
            fprintf(stream, "    -%s <int>\n", flag->name);
            fprintf(stream, "        %s\n", flag->desc);
            fprintf(stream, "        Default: %zu\n", flag->def.as_size);
            break;
        case FLAG_STR:
            fprintf(stream, "    -%s <str>\n", flag->name);
            fprintf(stream, "        %s\n", flag->desc);
            if (flag->def.as_str) {
                fprintf(stream, "        Default: %s\n", flag->def.as_str);
            }
            break;
        case COUNT_FLAG_TYPES:
        default:
            assert(0 && "unreachable");
            exit(69);
        }
    }
}

void flag_print_options(FILE *stream)
{
    flag_c_print_options(&flag_global_context, stream);
}

void flag_c_print_error(void *c, FILE *stream)
{
    Flag_Context *fc = (Flag_Context *)c;
    static_assert(COUNT_FLAG_ERRORS == 8, "Exhaustive flag error printing");
    switch (fc->flag_error) {
    case FLAG_NO_ERROR:
        // NOTE: don't call flag_print_error() if flag_parse() didn't return false, okay? ._.
        fprintf(stream, "Operation Failed Successfully! Please tell the developer of this software that they don't know what they are doing! :)");
        break;
    case FLAG_ERROR_UNKNOWN:
        fprintf(stream, "ERROR: -%s: unknown flag\n", fc->flag_error_name);
        break;
    case FLAG_ERROR_NO_VALUE:
        fprintf(stream, "ERROR: -%s: no value provided\n", fc->flag_error_name);
        break;
    case FLAG_ERROR_INVALID_NUMBER:
        fprintf(stream, "ERROR: -%s: invalid number\n", fc->flag_error_name);
        break;
    case FLAG_ERROR_INTEGER_OVERFLOW:
        fprintf(stream, "ERROR: -%s: integer overflow\n", fc->flag_error_name);
        break;
    case FLAG_ERROR_FLOAT_OVERFLOW:
        fprintf(stream, "ERROR: -%s: float overflow\n", fc->flag_error_name);
        break;
    case FLAG_ERROR_DOUBLE_OVERFLOW:
        fprintf(stream, "ERROR: -%s: double overflow\n", fc->flag_error_name);
        break;
    case FLAG_ERROR_INVALID_SIZE_SUFFIX:
        fprintf(stream, "ERROR: -%s: invalid size suffix\n", fc->flag_error_name);
        break;
    case COUNT_FLAG_ERRORS:
    default:
        assert(0 && "unreachable");
        exit(69);
    }
}

void flag_print_error(FILE *stream)
{
    flag_c_print_error(&flag_global_context, stream);
}

#endif // FLAG_IMPLEMENTATION

/*
   Revision history:

     1.7.1 (2025-10-24) Remove flag_error_value from Flag_Context (@rexim)
     1.7.0 (2025-09-27) Add float and double flags (by @ByXeno)
                        Add more size suffixes (by @ByXeno)
     1.6.1 (2025-09-23) Remove use_ref from Flag_Context
     1.6.0 (2025-09-22) Introduce *_var variants of flag functions
     1.5.0 (2025-09-22) Introduce -/flag syntax for ignoring flags
     1.4.1 (2025-09-05) Fix -Wswitch-enum warning for GCC/Clang
     1.4.0 (2025-07-23) Add support for explicit flag contexts
                        Add Flag_List_Mut
                        Implement `-key=value` syntax for flag lists
     1.3.0 (2025-07-21) Add support for `-key=value` syntax (by @ej-shafran)
     1.2.1 (2025-07-04) flag_print_options: denote expected argument types
                        flag_print_options: indicate flag list usage more clearly
     1.2.0 (2025-05-31) Introduce FLAG_PUSH_DASH_DASH_BACK (by @nullnominal)
     1.1.0 (2025-05-09) Introduce flag list
     1.0.0 (2025-03-03) Initial release
                        Save program_name in the context

*/

// Copyright 2021 Alexey Kutepov <reximkut@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
