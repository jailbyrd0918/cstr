#pragma once

#if	!defined(bool)
#include <stdbool.h>
#endif

#if	!defined(size_t)
#include <stdlib.h>
#endif


typedef	char *	string_t;


string_t
cstr_new
(const char *string);

string_t
cstr_reserve
(const size_t capacity);

void
cstr_destroy
(string_t string);


void
cstr_clear
(string_t string);

string_t
cstr_shrink_to_fit
(string_t string);

void
cstr_resize
(string_t string, size_t n, char c);

string_t
cstr_append
(string_t string, const char *append_str);

string_t
cstr_append_n
(string_t string, const char *append_str, size_t n);

string_t
cstr_append_string
(string_t string, const string_t append_str);

string_t
cstr_append_substring
(string_t string, const string_t append_str, size_t subpos, size_t sublen);

string_t
cstr_append_range
(string_t string, const void *start, const void *end);

string_t
cstr_append_fill
(string_t string, size_t n, char c);

void
cstr_push_back
(string_t string, char c);

void
cstr_pop_back
(string_t string);

string_t
cstr_assign
(string_t string, const char *assign_str);

string_t
cstr_assign_n
(string_t string, const char *assign_str, size_t n);

string_t
cstr_assign_string
(string_t string, const string_t assign_str);

string_t
cstr_assign_substring
(string_t string, const string_t assign_str, size_t subpos, size_t sublen);

string_t
cstr_assign_fill
(string_t string, size_t n, char c);

string_t
cstr_assign_range
(string_t string, const void *start, const void *end);

string_t
cstr_insert
(string_t string, size_t pos, const char *insert_str);

string_t
cstr_insert_n
(string_t string, size_t pos, const char *insert_str, size_t n);

string_t
cstr_insert_string
(string_t string, size_t pos, const string_t insert_str);

string_t
cstr_insert_substring
(string_t string, size_t pos, const string_t insert_str, size_t subpos, size_t sublen);

string_t
cstr_insert_fill
(string_t string, size_t pos, size_t n, char c);

string_t
cstr_insert_range
(string_t string, size_t pos, const void *start, const void *end);

string_t
cstr_erase
(string_t string, size_t pos, size_t len);

string_t
cstr_erase_range
(string_t string, const void *start, const void *end);

string_t
cstr_replace
(string_t string, size_t pos, size_t len, const char *replace_str);

string_t
cstr_replace_n
(string_t string, size_t pos, size_t len, const char *replace_str, size_t n);

string_t
cstr_replace_string
(string_t string, size_t pos, size_t len, const string_t replace_str);

string_t
cstr_replace_substring
(string_t string, size_t pos, size_t len, const string_t replace_str, size_t subpos, size_t sublen);

string_t
cstr_replace_fill
(string_t string, size_t pos, size_t len, size_t n, char c);

string_t
cstr_replace_range
(string_t string, size_t pos, size_t len, const void *start, const void *end);

void
cstr_swap
(string_t *left, string_t *right);

size_t
cstr_copy
(string_t string, char *s, size_t len, size_t pos);

size_t
cstr_find
(string_t string, const char *find_str, size_t pos);

size_t
cstr_find_n
(string_t string, const char *find_str, size_t pos, size_t n);

size_t
cstr_find_string
(string_t string, const string_t find_str, size_t pos);

size_t
cstr_find_char
(string_t string, char c, size_t pos);

size_t
cstr_rfind
(string_t string, const char *find_str, size_t pos);

size_t
cstr_rfind_n
(string_t string, const char *find_str, size_t pos, size_t n);

size_t
cstr_rfind_string
(string_t string, const string_t find_str, size_t pos);

size_t
cstr_rfind_char
(string_t string, char c, size_t pos);

size_t
cstr_find_first_of
(string_t string, const char *find_str, size_t pos);

size_t
cstr_find_first_of_n
(string_t string, const char *find_str, size_t pos, size_t n);

size_t
cstr_find_first_of_string
(string_t string, const string_t find_str, size_t pos);

size_t
cstr_find_first_of_char
(string_t string, char c, size_t pos);

size_t
cstr_find_last_of
(string_t string, const char *find_str, size_t pos);

size_t
cstr_find_last_of_n
(string_t string, const char *find_str, size_t pos, size_t n);

size_t
cstr_find_last_of_string
(string_t string, const string_t find_str, size_t pos);

size_t
cstr_find_last_of_char
(string_t string, char c, size_t pos);

size_t
cstr_find_first_not_of
(string_t string, const char *find_str, size_t pos);

size_t
cstr_find_first_not_of_n
(string_t string, const char *find_str, size_t pos, size_t n);

size_t
cstr_find_first_not_of_string
(string_t string, const string_t find_str, size_t pos);

size_t
cstr_find_first_not_of_char
(string_t string, char c, size_t pos);

size_t
cstr_find_last_not_of
(string_t string, const char *find_str, size_t pos);

size_t
cstr_find_last_not_of_n
(string_t string, const char *find_str, size_t pos, size_t n);

size_t
cstr_find_last_not_of_string
(string_t string, const string_t find_str, size_t pos);

size_t
cstr_find_last_not_of_char
(string_t string, char c, size_t pos);

string_t
cstr_substr
(string_t string, size_t pos, size_t len);

int
cstr_compare
(string_t string, const char *compare_str);

int
cstr_compare_ext
(string_t string, size_t pos, size_t len, const char *compare_str);

int
cstr_compare_n
(string_t string, const char *compare_str, size_t n);

int
cstr_compare_n_ext
(string_t string, size_t pos, size_t len, const char *compare_str, size_t n);

int
cstr_compare_string
(string_t string, const string_t compare_str);

int
cstr_compare_string_ext
(string_t string, size_t pos, size_t len, const string_t compare_str);

int
cstr_compare_substring
(string_t string, const string_t compare_str, size_t subpos, size_t sublen);

int
cstr_compare_substring_ext
(string_t string, size_t pos, size_t len, const string_t compare_str, size_t subpos, size_t sublen);


size_t
cstr_size
(string_t string);

size_t
cstr_length
(string_t string);

size_t
cstr_max_size
(string_t string);

size_t
cstr_capacity
(string_t string);

void *
cstr_begin
(string_t string);

void *
cstr_end
(string_t string);

bool
cstr_empty
(string_t string);

char
cstr_at
(string_t string, const size_t pos);

char
cstr_front
(string_t string);

char
cstr_back
(string_t string);

char *
cstr_data
(string_t string);




