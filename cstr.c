#include "cstr.h"

#include <string.h>
#include <limits.h>
#include <assert.h>

#define CSTR_DEFAULT_CAPACITY	10

#if	defined(_MSC_VER)
#	pragma	warning	(disable : 4996)
#	define	_CRT_SECURE_NO_WARNINGS
#endif


typedef struct string_header_t
{
	size_t	_size;
	size_t	_capacity;

	void *	_begin;
	void *	_end;
}
string_header_t;


string_header_t *
_cstr_header
(string_t string)
{
	return (string_header_t *)(string - sizeof(string_header_t));
}

string_t
_cstr_expand
(string_t string, const size_t size)
{
	string_header_t *header = _cstr_header(string);

	size_t new_capacity = header->_capacity;
	while (new_capacity < size)
		new_capacity <<= 1;

	size_t realloc_size = sizeof(string_header_t) + new_capacity;
	void *new_string = realloc(header, realloc_size);
	assert(new_string && "failed to resize string and allocate memory!");

	string_header_t *new_header = new_string;
	new_header->_capacity = new_capacity;
	new_header->_size = size;
	new_header->_begin = (string_t)new_string + sizeof(string_header_t);
	new_header->_end = (string_t)(new_header->_begin) + (size - 1);

	return (new_header->_begin);
}

bool
_cstr_invalid
(string_t string)
{
	string_header_t *header = _cstr_header(string);
	return (header->_capacity == 0) && (header->_begin == NULL);
}


string_t
cstr_new
(const char *string)
{
	assert(string && "string argument must be valid!");

	size_t length = strlen(string);

	size_t capacity = CSTR_DEFAULT_CAPACITY;
	while (capacity < length)
		capacity <<= 1;

	size_t alloc_size = sizeof(string_header_t) + capacity;

	void *new_string = malloc(alloc_size);
	assert(new_string && "failed to allocate new string!");

	string_header_t *header = new_string;
	header->_size = length;
	header->_capacity = capacity;

	header->_begin = (string_t)(header)+sizeof(string_header_t);
	strcpy(header->_begin, string);

	header->_end = (string_t)header->_begin + (header->_size - 1);

	return (header->_begin);
}

string_t
cstr_reserve
(const size_t capacity)
{
	assert((capacity > 0) && "capacity must be greater than zero!");

	size_t alloc_size = sizeof(string_header_t) + capacity;

	void *new_string = malloc(alloc_size);
	assert(new_string && "failed to allocate new string!");

	string_header_t *header = new_string;
	header->_size = 0;
	header->_capacity = capacity;
	header->_begin = (string_t)(header)+sizeof(string_header_t);
	header->_end = (string_t)header->_begin + (header->_size - 1);

	memset(header->_begin, 0, header->_size);

	return (header->_begin);
}

void
cstr_destroy
(string_t string)
{
	string_header_t *header = _cstr_header(string);
	assert(header && "failed to locate header address!");

	free(header);
}


void
cstr_clear
(string_t string)
{
	string_header_t *header = _cstr_header(string);
	memset(header->_begin, 0, header->_size);
	header->_size = 0;
}

string_t
cstr_shrink_to_fit
(string_t string)
{
	string_header_t *old_header = _cstr_header(string);
	size_t size = old_header->_size;

	size_t realloc_size = sizeof(string_header_t) + (size + 1);
	void *new_string = realloc(old_header, realloc_size);
	assert(new_string && "failed to shrink string to fit!");

	string_header_t *new_header = new_string;
	new_header->_capacity = size;

	new_header->_begin = (string_t)(new_header)+sizeof(string_header_t);
	new_header->_end = (string_t)(new_header->_begin) + (size - 1);

	((char *)new_header->_end)[1] = '\0';
	return (new_header->_begin);
}

void
cstr_resize
(string_t string, size_t n, char c)
{
	string_header_t *header = _cstr_header(string);
	size_t old_size = header->_size;

	if (n > header->_capacity) {
		string = _cstr_expand(string, n);
		header = _cstr_header(string);

		for (size_t i = old_size; i < header->_size; ++i)
			((char *)header->_begin)[i] = c;

		((char *)header->_end)[1] = '\0';
	}
	else {
		header->_size = n;
		header->_end = (string_t)(header->_begin) + (n - 1);
		((char *)header->_end)[1] = '\0';
	}
}

string_t
cstr_append
(string_t string, const char *append_str)
{
	assert(append_str && "append_str argument must be valid!");

	string_header_t *header = _cstr_header(string);
	size_t old_size = header->_size;
	size_t append_length = strlen(append_str);
	size_t new_size = old_size + append_length;

	if (new_size >= header->_capacity) {
		string = _cstr_expand(string, new_size);
		header = _cstr_header(string);
	}
	else {
		header->_size = new_size;
		header->_end = (string_t)(header->_begin) + (new_size - 1);
	}

	memcpy((char *)header->_begin + old_size, append_str, append_length);

	((char *)header->_end)[1] = '\0';
	return (header->_begin);
}

string_t
cstr_append_n
(string_t string, const char *append_str, size_t n)
{
	assert(append_str && "append_str argument must be valid!");
	assert((n <= strlen(append_str)) && "n should be less than or equal to length of append_str!");

	string_header_t *header = _cstr_header(string);
	size_t old_size = header->_size;
	size_t new_size = old_size + n;

	if (new_size >= header->_capacity) {
		string = _cstr_expand(string, new_size);
		header = _cstr_header(string);
	}
	else {
		header->_size = new_size;
		header->_end = (string_t)(header->_begin) + (new_size - 1);
	}

	for (size_t i = 0; i < n; ++i)
		((char *)header->_begin)[old_size + i] = append_str[i];

	((char *)header->_end)[1] = '\0';
	return (header->_begin);
}

string_t
cstr_append_string
(string_t string, const string_t append_str)
{
	assert(append_str && "append_str argument must be valid!");

	return cstr_append(string, cstr_data(append_str));
}

string_t
cstr_append_substring
(string_t string, const string_t append_str, size_t subpos, size_t sublen)
{
	assert(append_str && "append_str argument must be valid!");

	size_t length = cstr_length(append_str);
	if (subpos < length) {
		size_t actual_sublen = ((sublen > (length - subpos)) ? (length - subpos) : sublen);
		return cstr_append_n(string, cstr_data(append_str) + subpos, actual_sublen);
	}

	return string;
}

string_t
cstr_append_fill
(string_t string, size_t n, char c)
{
	string_header_t *header = _cstr_header(string);
	size_t old_size = header->_size;
	size_t new_size = old_size + n;

	if (new_size >= header->_capacity) {
		string = _cstr_expand(string, new_size);
		header = _cstr_header(string);
	}
	else {
		header->_size = new_size;
		header->_end = (string_t)(header->_begin) + (new_size - 1);
	}

	for (size_t i = old_size; i < new_size; ++i)
		((char *)header->_begin)[i] = c;

	((char *)header->_end)[1] = '\0';
	return (header->_begin);
}

string_t
cstr_append_range
(string_t string, const void *start, const void *end)
{
	string_header_t *header = _cstr_header(string);
	size_t old_size = header->_size;
	size_t new_size = old_size + (ptrdiff_t)((char *)end - (char *)start);

	if (new_size >= header->_capacity) {
		string = _cstr_expand(string, new_size);
		header = _cstr_header(string);
	}
	else {
		header->_size = new_size;
		header->_end = (string_t)(header->_begin) + (new_size - 1);
	}

	char *itr = (char *)start;
	for (size_t i = 0; itr <= (char *)end; ++itr, ++i)
		((char *)header->_begin)[old_size + i] = *itr;

	return (header->_begin);
}

void
cstr_push_back
(string_t string, char c)
{
	string_header_t *header = _cstr_header(string);
	if (header->_size >= header->_capacity) {
		string = _cstr_expand(string, header->_size + 1);
		header = _cstr_header(string);
	}
	else {
		header->_size++;
		header->_end = (string_t)(header->_begin) + (header->_size - 1);
	}

	((char *)header->_end)[0] = c;
	((char *)header->_end)[1] = '\0';
}

void
cstr_pop_back
(string_t string)
{
	string_header_t *header = _cstr_header(string);
	header->_size--;
	header->_end = (string_t)(header->_begin) + (header->_size - 1);

	((char *)header->_end)[1] = '\0';
}

string_t
cstr_assign
(string_t string, const char *assign_str)
{
	assert(assign_str && "assign_str argument must be valid!");

	cstr_clear(string);
	return cstr_append(string, assign_str);
}

string_t
cstr_assign_n
(string_t string, const char *assign_str, size_t n)
{
	assert(assign_str && "assign_str argument must be valid!");

	cstr_clear(string);
	return cstr_append_n(string, assign_str, n);
}

string_t
cstr_assign_string
(string_t string, const string_t assign_str)
{
	assert(assign_str && "assign_str argument must be valid!");

	cstr_clear(string);
	return cstr_append_string(string, assign_str);
}

string_t
cstr_assign_substring
(string_t string, const string_t assign_str, size_t subpos, size_t sublen)
{
	assert(assign_str && "assign_str argument must be valid!");

	cstr_clear(string);
	return cstr_append_substring(string, assign_str, subpos, sublen);
}

string_t
cstr_assign_fill
(string_t string, size_t n, char c)
{
	cstr_clear(string);
	return cstr_append_fill(string, n, c);
}

string_t
cstr_assign_range
(string_t string, const void *start, const void *end)
{
	cstr_clear(string);
	return cstr_append_range(string, start, end);
}

string_t
cstr_insert
(string_t string, size_t pos, const char *insert_str)
{
	assert(insert_str && "insert_str argument must be valid!");
	assert((pos <= cstr_size(string)) && "pos argument is out of range!");

	string_header_t *header = _cstr_header(string);
	size_t old_size = header->_size;
	size_t insert_length = strlen(insert_str);
	size_t new_size = old_size + insert_length;

	if (new_size >= header->_capacity) {
		string = _cstr_expand(string, new_size);
		header = _cstr_header(string);
	}
	else {
		header->_size = new_size;
		header->_end = (string_t)(header->_begin) + (new_size - 1);
	}

	size_t move_size = old_size - pos;
	memmove(
		(string_t)(header->_begin) + (pos + insert_length),
		(string_t)(header->_begin) + pos,
		move_size
	);

	memcpy((string_t)(header->_begin) + pos, insert_str, insert_length);

	((char *)header->_end)[1] = '\0';
	return (header->_begin);
}

string_t
cstr_insert_n
(string_t string, size_t pos, const char *insert_str, size_t n)
{
	assert(insert_str && "insert_str argument must be valid!");
	assert((pos <= cstr_size(string)) && "pos argument is out of range!");

	string_header_t *header = _cstr_header(string);
	size_t old_size = header->_size;
	size_t new_size = old_size + n;

	if (new_size >= header->_capacity) {
		string = _cstr_expand(string, new_size);
		header = _cstr_header(string);
	}
	else {
		header->_size = new_size;
		header->_end = (string_t)(header->_begin) + (new_size - 1);
	}

	size_t move_size = old_size - pos;
	memmove(
		(string_t)(header->_begin) + (pos + n),
		(string_t)(header->_begin) + pos,
		move_size
	);

	memcpy((string_t)(header->_begin) + pos, insert_str, n);

	((char *)header->_end)[1] = '\0';
	return (header->_begin);
}

string_t
cstr_insert_string
(string_t string, size_t pos, const string_t insert_str)
{
	assert(insert_str && "insert_str argument must be valid!");

	return cstr_insert(string, pos, cstr_data(insert_str));
}

string_t
cstr_insert_substring
(string_t string, size_t pos, const string_t insert_str, size_t subpos, size_t sublen)
{
	assert(insert_str && "insert_str argument must be valid!");

	size_t length = cstr_length(insert_str);
	if (subpos < length) {
		size_t actual_sublen = ((sublen > (length - subpos)) ? (length - subpos) : sublen);
		return cstr_insert_n(string, pos, cstr_data(insert_str) + subpos, actual_sublen);
	}

	return string;
}

string_t
cstr_insert_fill
(string_t string, size_t pos, size_t n, char c)
{
	string_header_t *header = _cstr_header(string);
	size_t old_size = header->_size;
	size_t new_size = old_size + n;

	if (new_size >= header->_capacity) {
		string = _cstr_expand(string, new_size);
		header = _cstr_header(string);
	}
	else {
		header->_size = new_size;
		header->_end = (string_t)(header->_begin) + (new_size - 1);
	}

	size_t move_size = old_size - pos;
	memmove(
		(string_t)(header->_begin) + (pos + n),
		(string_t)(header->_begin) + pos,
		move_size
	);

	for (size_t i = pos; i < pos + n; ++i)
		((char *)header->_begin)[i] = c;

	((char *)header->_end)[1] = '\0';
	return (header->_begin);
}

string_t
cstr_insert_range
(string_t string, size_t pos, const void *start, const void *end)
{
	string_header_t *header = _cstr_header(string);
	size_t old_size = header->_size;
	size_t insert_length = (ptrdiff_t)((char *)end - (char *)start);
	size_t new_size = old_size + insert_length;

	if (new_size >= header->_capacity) {
		string = _cstr_expand(string, new_size);
		header = _cstr_header(string);
	}
	else {
		header->_size = new_size;
		header->_end = (string_t)(header->_begin) + (new_size - 1);
	}

	size_t move_size = old_size - pos;
	memmove(
		(string_t)(header->_begin) + (pos + insert_length),
		(string_t)(header->_begin) + pos,
		move_size
	);

	memcpy((string_t)(header->_begin) + pos, start, insert_length);

	((char *)header->_end)[1] = '\0';
	return (header->_begin);
}

string_t
cstr_erase
(string_t string, size_t pos, size_t len)
{
	assert((pos <= cstr_size(string)) && "'pos' argument is out of range!");
	assert((len <= (cstr_size(string) - pos)) && "'len' argument exceeds limit!");

	string_header_t *header = _cstr_header(string);
	size_t old_size = header->_size;
	size_t new_size = old_size - len;

	if (new_size >= header->_capacity) {
		string = _cstr_expand(string, new_size);
		header = _cstr_header(string);
	}
	else {
		header->_size = new_size;
		header->_end = (string_t)(header->_begin) + (new_size - 1);
	}

	size_t move_size = old_size - pos;
	memmove(
		(string_t)(header->_begin) + pos,
		(string_t)(header->_begin) + (pos + len),
		move_size
	);

	((char *)header->_end)[1] = '\0';
	return (header->_begin);
}

string_t
cstr_erase_range
(string_t string, const void *start, const void *end)
{
	string_header_t *header = _cstr_header(string);
	size_t old_size = header->_size;
	size_t erase_length = (ptrdiff_t)((char *)end - (char *)start);
	size_t new_size = old_size - erase_length;

	if (new_size >= header->_capacity) {
		string = _cstr_expand(string, new_size);
		header = _cstr_header(string);
	}
	else {
		header->_size = new_size;
		header->_end = (string_t)(header->_begin) + (new_size - 1);
	}

	size_t move_size = old_size - erase_length;
	string_t move_dest = (string_t)start;
	memmove(move_dest, end, move_size);

	((char *)header->_end)[1] = '\0';
	return (header->_begin);
}

string_t
cstr_replace
(string_t string, size_t pos, size_t len, const char *replace_str)
{
	assert(replace_str && "replace_str argument must be valid!");
	assert((pos <= cstr_size(string)) && "pos argument is out of range!");
	assert((len <= (cstr_size(string) - pos)) && "'len' argument exceed limit!");

	string_header_t *header = _cstr_header(string);
	size_t old_size = header->_size;
	size_t replace_length = strlen(replace_str);
	size_t new_size = old_size - len + replace_length;

	if (new_size >= header->_capacity) {
		string = _cstr_expand(string, new_size);
		header = _cstr_header(string);
	}
	else {
		header->_size = new_size;
		header->_end = (string_t)(header->_begin) + (new_size - 1);
	}

	size_t move_size = old_size - (pos + len);
	memmove(
		(string_t)(header->_begin) + (pos + replace_length),
		(string_t)(header->_begin) + (pos + len),
		move_size
	);

	memcpy((string_t)(header->_begin) + pos, replace_str, replace_length);
	((char *)header->_end)[1] = '\0';
	return (header->_begin);
}

string_t
cstr_replace_n
(string_t string, size_t pos, size_t len, const char *replace_str, size_t n)
{
	assert(replace_str && "replace_str argument must be valid!");
	assert((pos <= cstr_size(string)) && "pos argument is out of range!");
	assert((len <= (cstr_size(string) - pos)) && "'len' argument exceed limit!");
	assert((n <= strlen(replace_str)) && "'n' argument exceed replace_str length!");

	string_header_t *header = _cstr_header(string);
	size_t old_size = header->_size;
	size_t new_size = old_size - len + n;

	if (new_size >= header->_capacity) {
		string = _cstr_expand(string, new_size);
		header = _cstr_header(string);
	}
	else {
		header->_size = new_size;
		header->_end = (string_t)(header->_begin) + (new_size - 1);
	}

	size_t move_size = old_size - (pos + len);
	memmove(
		(string_t)(header->_begin) + (pos + n),
		(string_t)(header->_begin) + (pos + len),
		move_size
	);

	memcpy((string_t)(header->_begin) + pos, replace_str, n);
	((char *)header->_end)[1] = '\0';
	return (header->_begin);
}

string_t
cstr_replace_string
(string_t string, size_t pos, size_t len, const string_t replace_str)
{
	assert(replace_str && "replace_str argument must be valid!");
	assert((pos <= cstr_size(string)) && "pos argument is out of range!");
	assert((len <= (cstr_size(string) - pos)) && "'len' argument exceed limit!");

	return cstr_replace(string, pos, len, cstr_data(replace_str));
}

string_t
cstr_replace_substring
(string_t string, size_t pos, size_t len, const string_t replace_str, size_t subpos, size_t sublen)
{
	assert(replace_str && "replace_str argument must be valid!");

	size_t length = cstr_length(replace_str);
	if (subpos < length) {
		size_t actual_sublen = ((sublen > (length - subpos)) ? (length - subpos) : sublen);
		return cstr_replace_n(string, pos, len, cstr_data(replace_str) + subpos, actual_sublen);
	}

	return string;
}

string_t
cstr_replace_fill
(string_t string, size_t pos, size_t len, size_t n, char c)
{
	assert((pos <= cstr_size(string)) && "pos argument is out of range!");
	assert((len <= (cstr_size(string) - pos)) && "'len' argument exceed limit!");

	string_header_t *header = _cstr_header(string);
	size_t old_size = header->_size;
	size_t new_size = old_size - len + n;

	if (new_size >= header->_capacity) {
		string = _cstr_expand(string, new_size);
		header = _cstr_header(string);
	}
	else {
		header->_size = new_size;
		header->_end = (string_t)(header->_begin) + (new_size - 1);
	}

	size_t move_size = old_size - (pos + len);
	memmove(
		(string_t)(header->_begin) + (pos + n),
		(string_t)(header->_begin) + (pos + len),
		move_size
	);

	for (size_t i = pos; i < pos + n; ++i)
		((char *)header->_begin)[i] = c;

	((char *)header->_end)[1] = '\0';
	return (header->_begin);
}

string_t
cstr_replace_range
(string_t string, size_t pos, size_t len, const void *start, const void *end)
{
	string_header_t *header = _cstr_header(string);
	size_t old_size = header->_size;
	size_t replace_length = (ptrdiff_t)((char *)end - (char *)start);
	size_t new_size = old_size - len + replace_length;

	if (new_size >= header->_capacity) {
		string = _cstr_expand(string, new_size);
		header = _cstr_header(string);
	}
	else {
		header->_size = new_size;
		header->_end = (string_t)(header->_begin) + (new_size - 1);
	}

	size_t move_size = old_size - (pos + len);
	memmove(
		(string_t)(header->_begin) + (pos + replace_length),
		(string_t)(header->_begin) + (pos + len),
		move_size
	);

	memcpy((string_t)(header->_begin) + pos, start, replace_length);

	((char *)header->_end)[1] = '\0';
	return (header->_begin);
}

void
cstr_swap
(string_t *left, string_t *right)
{
	if (_cstr_invalid(*left))
		*left = cstr_new(*left);

	if (_cstr_invalid(*right))
		*right = cstr_new(*right);

	string_t temp = *left;
	*left = *right;
	*right = temp;
}

size_t
cstr_copy
(string_t string, char *s, size_t len, size_t pos)
{
	assert((pos <= cstr_size(string)) && "pos argument is out of range!");
	assert((len <= (cstr_size(string) - pos)) && "'len' argument exceed limit!");

	string_header_t *header = _cstr_header(string);

	memcpy(s, (string_t)(header->_begin) + pos, len);
	s[len] = '\0';

	return strlen(s);
}

size_t
cstr_find
(string_t string, const char *find_str, size_t pos)
{
	assert(find_str && "find_str argument must be valid!");
	assert((pos <= cstr_size(string)) && "'pos' argument is out of range!");

	return (strstr(string + pos, find_str) - string);
}

size_t
cstr_find_n
(string_t string, const char *find_str, size_t pos, size_t n)
{
	assert(find_str && "find_str argument must be valid!");
	assert((pos <= cstr_size(string)) && "'pos' argument is out of range!");

	char *actual_find_str = strdup(find_str);
	actual_find_str[n] = '\0';

	size_t return_pos = cstr_find(string, actual_find_str, pos);

	free(actual_find_str);
	return return_pos;
}

size_t
cstr_find_string
(string_t string, const string_t find_str, size_t pos)
{
	return cstr_find(string, cstr_data(find_str), pos);
}

size_t
cstr_find_char
(string_t string, char c, size_t pos)
{
	assert((pos <= cstr_size(string)) && "'pos' argument is out of range!");

	return (strchr(string + pos, c) - string);
}

size_t
cstr_rfind
(string_t string, const char *find_str, size_t pos)
{
	assert(find_str && "find_str argument must be valid!");
	assert((pos <= cstr_size(string)) && "'pos' argument is out of range!");

	char *ptr = string;
	char *last = NULL;

	while ((ptr = strstr(ptr, find_str)) != NULL)
		last = ptr++;

	return (last - string);
}

size_t
cstr_rfind_n
(string_t string, const char *find_str, size_t pos, size_t n)
{
	assert(find_str && "find_str argument must be valid!");
	assert((pos <= cstr_size(string)) && "'pos' argument is out of range!");

	char *ptr = string;
	char *last = NULL;

	char *actual_find_str = strdup(find_str);
	actual_find_str[n] = '\0';

	while ((ptr = strstr(ptr, actual_find_str)) != NULL)
		last = ptr++;

	free(actual_find_str);
	return (last - string);
}

size_t
cstr_rfind_string
(string_t string, const string_t find_str, size_t pos)
{
	return cstr_rfind(string, cstr_data(find_str), pos);
}

size_t
cstr_rfind_char
(string_t string, char c, size_t pos)
{
	assert((pos <= cstr_size(string)) && "'pos' argument is out of range!");

	return (strrchr(string + pos, c) - string);
}

size_t
cstr_find_first_of
(string_t string, const char *find_str, size_t pos)
{
	assert(find_str && "find_str argument must be valid!");
	assert((pos <= cstr_size(string)) && "'pos' argument is out of range!");

	return (strpbrk(string + pos, find_str) - string);
}

size_t
cstr_find_first_of_n
(string_t string, const char *find_str, size_t pos, size_t n)
{
	assert(find_str && "find_str argument must be valid!");
	assert((pos <= cstr_size(string)) && "'pos' argument is out of range!");

	char *actual_find_str = strdup(find_str);
	actual_find_str[n] = '\0';

	size_t return_size = strpbrk(string + pos, actual_find_str) - string;

	free(actual_find_str);
	return return_size;
}

size_t
cstr_find_first_of_string
(string_t string, const string_t find_str, size_t pos)
{
	return cstr_find_first_of(string, cstr_data(find_str), pos);
}

size_t
cstr_find_first_of_char
(string_t string, char c, size_t pos)
{
	return cstr_find_char(string, c, pos);
}

size_t
cstr_find_last_of
(string_t string, const char *find_str, size_t pos)
{
	assert(find_str && "find_str argument must be valid!");
	assert((pos <= cstr_size(string)) && "'pos' argument is out of range!");

	size_t last_most = 0;
	for (size_t i = 0; i < strlen(find_str); ++i) {
		size_t last = strrchr(string + pos, find_str[i]) - string;

		if ((last > last_most) && (last < cstr_size(string)))
			last_most = last;
	}

	return last_most;
}

size_t
cstr_find_last_of_n
(string_t string, const char *find_str, size_t pos, size_t n)
{
	assert(find_str && "find_str argument must be valid!");
	assert((pos <= cstr_size(string)) && "'pos' argument is out of range!");

	size_t last_most = 0;
	for (size_t i = 0; i < n; ++i) {
		size_t last = strrchr(string + pos, find_str[i]) - string;

		if ((last > last_most) && (last < cstr_size(string)))
			last_most = last;
	}

	return last_most;
}

size_t
cstr_find_last_of_string
(string_t string, const string_t find_str, size_t pos)
{
	return cstr_find_last_of(string, cstr_data(find_str), pos);
}

size_t
cstr_find_last_of_char
(string_t string, char c, size_t pos)
{
	return cstr_rfind_char(string, c, pos);
}

size_t
cstr_find_first_not_of
(string_t string, const char *find_str, size_t pos)
{
	assert(find_str && "find_str argument must be valid!");
	assert((pos <= cstr_size(string)) && "'pos' argument is out of range!");

	for (size_t i = pos; i < cstr_size(string); ++i) {
		char *current = strchr(find_str, string[i]);
		
		if (!current)
			return i;
	}

	return cstr_max_size(string);
}

size_t
cstr_find_first_not_of_n
(string_t string, const char *find_str, size_t pos, size_t n)
{
	assert(find_str && "find_str argument must be valid!");
	assert((pos <= cstr_size(string)) && "'pos' argument is out of range!");

	char *actual_find_str = strdup(find_str);
	actual_find_str[n] = '\0';

	for (size_t i = pos; i < cstr_size(string); ++i) {
		char *current = strchr(actual_find_str, string[i]);

		if (!current) {
			free(actual_find_str);
			return i;
		}
	}

	free(actual_find_str);
	return cstr_max_size(string);
}

size_t
cstr_find_first_not_of_string
(string_t string, const string_t find_str, size_t pos)
{
	return cstr_find_first_not_of(string, cstr_data(find_str), pos);
}

size_t
cstr_find_first_not_of_char
(string_t string, char c, size_t pos)
{
	assert((pos <= cstr_size(string)) && "'pos' argument is out of range!");

	for (size_t i = pos; i < cstr_size(string); ++i)
		if (string[i] != c) return i;

	return cstr_max_size(string);
}

size_t
cstr_find_last_not_of
(string_t string, const char *find_str, size_t pos)
{
	assert(find_str && "find_str argument must be valid!");
	assert((pos <= cstr_size(string)) && "'pos' argument is out of range!");

	for (size_t i = cstr_size(string) - 1; i >= pos; --i) {
		char *current = strchr(find_str, string[i]);

		if (!current)
			return i;
	}

	return cstr_max_size(string);
}

size_t
cstr_find_last_not_of_n
(string_t string, const char *find_str, size_t pos, size_t n)
{
	assert(find_str && "find_str argument must be valid!");
	assert((pos <= cstr_size(string)) && "'pos' argument is out of range!");

	char *actual_find_str = strdup(find_str);
	actual_find_str[n] = '\0';

	for (size_t i = cstr_size(string) - 1; i >= pos; --i) {
		char *current = strchr(actual_find_str, string[i]);

		if (!current) {
			free(actual_find_str);
			return i;
		}
	}

	free(actual_find_str);
	return cstr_max_size(string);
}

size_t
cstr_find_last_not_of_string
(string_t string, const string_t find_str, size_t pos)
{
	return cstr_find_last_not_of(string, cstr_data(find_str), pos);
}

size_t
cstr_find_last_not_of_char
(string_t string, char c, size_t pos)
{
	assert((pos <= cstr_size(string)) && "'pos' argument is out of range!");

	for (size_t i = cstr_size(string) - 1; i >= pos; --i)
		if (string[i] != c) return i;

	return cstr_max_size(string);
}

string_t
cstr_substr
(string_t string, size_t pos, size_t len)
{
	assert((pos <= cstr_size(string)) && "'pos' argument is out of range!");

	char *actual_string = strdup(string + pos);
	actual_string[len] = '\0';

	string_t substr = cstr_new(actual_string);

	free(actual_string);
	return substr;
}

int
cstr_compare
(string_t string, const char *compare_str)
{	
	assert(compare_str && "compare_str argument must be valid!");

	return strcmp(cstr_data(string), compare_str);
}

int
cstr_compare_ext
(string_t string, size_t pos, size_t len, const char *compare_str)
{
	assert(compare_str && "compare_str argument must be valid!");

	return strncmp(cstr_data(string) + pos, compare_str, len);
}

int
cstr_compare_n
(string_t string, const char *compare_str, size_t n)
{
	assert(compare_str && "compare_str argument must be valid!");

	char *actual_compare_str = strdup(compare_str);
	actual_compare_str[n] = '\0';

	int res = strcmp(cstr_data(string), actual_compare_str);

	free(actual_compare_str);
	return res;
}

int
cstr_compare_n_ext
(string_t string, size_t pos, size_t len, const char *compare_str, size_t n)
{
	assert(compare_str && "compare_str argument must be valid!");

	char *actual_compare_str = strdup(compare_str);
	actual_compare_str[n] = '\0';

	int res = strncmp(cstr_data(string) + pos, actual_compare_str, len);

	free(actual_compare_str);
	return res;
}

int
cstr_compare_string
(string_t string, const string_t compare_str)
{
	return cstr_compare(string, cstr_data(compare_str));
}

int
cstr_compare_string_ext
(string_t string, size_t pos, size_t len, const string_t compare_str)
{
	return cstr_compare_ext(string, pos, len, cstr_data(compare_str));
}

int
cstr_compare_substring
(string_t string, const string_t compare_str, size_t subpos, size_t sublen)
{
	assert(compare_str && "compare_str argument must be valid!");

	size_t length = cstr_length(compare_str);
	if (subpos < length) {
		size_t actual_sublen = ((sublen > (length - subpos)) ? (length - subpos) : sublen);
		return cstr_compare_n(string, cstr_data(compare_str) + subpos, actual_sublen);
	}

	return cstr_max_size(string);
}

int
cstr_compare_substring_ext
(string_t string, size_t pos, size_t len, const string_t compare_str, size_t subpos, size_t sublen)
{
	assert(compare_str && "compare_str argument must be valid!");

	size_t length = cstr_length(compare_str);
	if (subpos < length) {
		size_t actual_sublen = ((sublen > (length - subpos)) ? (length - subpos) : sublen);
		return cstr_compare_n_ext(string, pos, len, cstr_data(compare_str) + subpos, actual_sublen);
	}

	return cstr_max_size(string);
}


size_t
cstr_size
(string_t string)
{
	return (_cstr_header(string)->_size);
}

size_t
cstr_length
(string_t string)
{
	return (_cstr_header(string)->_size);
}

size_t
cstr_max_size
(string_t string)
{
	return SIZE_MAX;
}

size_t
cstr_capacity
(string_t string)
{
	return (_cstr_header(string)->_capacity);
}

void *
cstr_begin
(string_t string)
{
	return (_cstr_header(string)->_begin);
}

void *
cstr_end
(string_t string)
{
	return (_cstr_header(string)->_end);
}

bool
cstr_empty
(string_t string)
{
	return ((_cstr_header(string)->_size) == 0);
}

char
cstr_at
(string_t string, const size_t pos)
{
	assert(
		(pos < _cstr_header(string)->_size) &&
		"attempted to access element at out of bound position!"
	);

	return (string[pos]);
}

char
cstr_front
(string_t string)
{
	return (*(char *)cstr_begin(string));
}

char
cstr_back
(string_t string)
{
	return (*(char *)cstr_end(string));
}

char *
cstr_data
(string_t string)
{
	return (char *)(cstr_begin(string));
}


