#ifndef TARANTOOL_CSV_H_INCLUDED
#define TARANTOOL_CSV_H_INCLUDED

#include<stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef void (*csv_emit_row_t)(void *ctx);
typedef void (*csv_emit_field_t)(void *ctx, const char *field, const char *end);

struct csv
{
	void *emit_ctx;
	csv_emit_row_t emit_row;
	csv_emit_field_t emit_field;
	char csv_delim;
	char csv_quote;

	int csv_invalid;
	int csv_ending_spaces;

	void *(*csv_realloc)(void*, size_t);

	int state;
	char *buf;
	char *bufp;
	size_t buf_len;
};

//parser options
enum {
	CSV_OPT_DELIMITER,
	CSV_OPT_QUOTE,
	CSV_OPT_REALLOC,
	CSV_OPT_EMIT_FIELD,
	CSV_OPT_EMIT_ROW,
	CSV_OPT_CTX
};

//iteraion states
enum {
	CSV_IT_OK,
	CSV_IT_EOL,
	CSV_IT_NEEDMORE,
	CSV_IT_EOF,
	CSV_IT_ERROR
};

//parser states
enum {  CSV_LEADING_SPACES,
	CSV_BUF_OUT_OF_QUOTES,
	CSV_BUF_IN_QUOTES,
	CSV_NEWLINE
};

void
csv_create(struct csv *csv);

void
csv_destroy(struct csv *csv);

/**
 * Set a parser option.
 */
void
csv_setopt(struct csv *csv, int opt, ...);

/**
 * Parse input and call emit_row/emit_line.
 * Save tail to inside buffer,
 * next call will concatenate tail and string from args
 */
void
csv_parse_chunk(struct csv *csv, const char *s, const char *end);


void
csv_finish_parsing(struct csv *csv);

/**
 * Format variadic arguments and print them into
 * a stream, adding CSV markup.
 */
int
csv_snprintf(struct csv *csv, FILE *f, const char *format, ...);

/**
 * if quote not closed returns 0
 */
int
csv_isvalid(struct csv *csv);


struct csv_iterator {
	struct csv *csv;

	const char *buf_begin;
	const char *buf_end;

	const char *field;
	size_t field_len;
};

void
csv_iter_create(struct csv_iterator *it, struct csv *csv);
/**
 * @brief csv_next recieves next element from csv
 * element is field or end of line
 * @return iteration state
 */
int
csv_next(struct csv_iterator *);

/**
 * @brief csv_feed delivers buffer to iterator
 * empty buffer means end of iteration
 */
void
csv_feed(struct csv_iterator *, const char *);

#define CSV_ITERATOR_GET_FIELD(it) it->field
#define CSV_ITERATOR_GET_FLEN(it)  it->field_len

#if defined(__cplusplus)
}
#endif /* extern "C" */
#endif

