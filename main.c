#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const uint32_t COLUMN_USERNAME_SIZE = 32;
const uint32_t COlUMN_EMAIL_SIZE = 255;

enum MetaCommandResult
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND,
};

enum PrepareResult
{
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT,
};

enum StatementType
{
    STATEMENT_INSERT,
    STATEMENT_SELECT,
};

struct Row
{
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COlUMN_EMAIL_SIZE];
};

struct Statement
{
    enum StatementType type;
    struct Row row_to_insert;
};

struct InputBuffer
{
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
};

#define size_of_attribute(s, a) sizeof(((s *)0)->a)

const uint32_t ID_SIZE = size_of_attribute(struct Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(struct Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(struct Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

void serialize_row(struct Row *source, void *destination)
{
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserialize_row(void *source, struct Row *destination)
{

    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
};
struct InputBuffer *new_input_buffer()
{
    struct InputBuffer *input_buffer = malloc(sizeof(struct InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

enum MetaCommandResult do_meta_command(struct InputBuffer *input_buffer)
{
    if (strcmp(input_buffer->buffer, ".exit") == 0)
    {
        exit(EXIT_SUCCESS);
    }
    else
    {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

enum PrepareResult prepare_statement(struct InputBuffer *input_buffer, struct Statement *statement)
{
    if (strncmp(input_buffer->buffer, "insert", 6) == 0)
    {
        statement->type = STATEMENT_INSERT;
        int args_assigned = sscanf(input_buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id), statement->row_to_insert.username, statement->row_to_insert.emal));
        if (args_assigned < 3)
        {
            return PREPARE_SYNTAX_ERROR;
        }
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "select") == 0)
    {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(struct Statement *statement)
{
    switch (statement->type)
    {
    case (STATEMENT_INSERT):
        printf("this is where we would do and insert. \n");
        break;
    case (STATEMENT_SELECT):
        printf("this is where we would do a select. \n");
        break;
    }
}

void print_prompt()
{
    printf("db >");
};

void read_input(struct InputBuffer *input_buffer)
{
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0)
    {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
}

int main(int argc, char *argv[])
{
    struct InputBuffer *input_buffer = new_input_buffer();

    while (true)
    {
        print_prompt();
        read_input(input_buffer);

        if (input_buffer->buffer[0] == '.')
        {
            switch (do_meta_command(input_buffer))
            {
            case (META_COMMAND_SUCCESS):
                continue;
            case (META_COMMAND_UNRECOGNIZED_COMMAND):
                printf("unrecognized command %s \n", input_buffer->buffer);
                continue;
            }
        }

        struct Statement statement;

        switch (prepare_statement(input_buffer, &statement))
        {
        case (PREPARE_SUCCESS):
            break;
        case (PREPARE_UNRECOGNIZED_STATEMENT):
            printf("unrecognized keywork at start of %s \n", input_buffer->buffer);
            continue;
        }

        execute_statement(&statement);
        printf("Executed. \n");
    }
}
