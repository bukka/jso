# JSO - JSON Parser and Schema Validator

A high-performance C library for JSON parsing, encoding, and JSON Schema validation.

## Overview

JSO is a comprehensive JSON processing library written in C that provides fast parsing, flexible encoding, and robust JSON Schema validation. The library is designed for high performance and memory efficiency, making it suitable for both embedded systems and high-throughput applications.

The library supports modern JSON Schema specifications and includes additional features like JSON Pointer support and a built-in command-line interface for standalone JSON processing tasks.

## Features

- **Fast JSON Parser**: Parse JSON from strings, files, or streams
- **JSON Schema Validation**: Support for JSON Schema Draft 4 and Draft 6
- **Flexible Encoding**: Output JSON in minimal, pretty-printed, or debug formats
- **JSON Pointer**: RFC 6901 compliant JSON Pointer implementation
- **Streaming Validation**: Validate JSON during parsing for memory efficiency
- **Command Line Interface**: Built-in CLI tool for JSON processing
- **Memory Efficient**: Designed for optimal memory usage and performance
- **Embeddable**: Virtual API layer allows for custom integrations (see Advanced Usage)

## Quick Start

### Basic JSON Parsing

```c
#include "jso.h"

int main() {
    const char *json_str = "{\"name\": \"John\", \"age\": 30}";
    jso_value result;
    jso_parser_options options;
    
    // Initialize parser options
    jso_parser_options_init(&options);
    
    // Parse JSON string
    if (jso_parse_cstr(json_str, strlen(json_str), &options, &result) == JSO_SUCCESS) {
        // Successfully parsed
        printf("Parsing successful!\n");
        
        // Clean up
        jso_value_free(&result);
    }
    
    return 0;
}
```

### JSON Schema Validation

```c
#include "jso.h"

int main() {
    // Schema definition
    const char *schema_str = R"({
        "type": "object",
        "properties": {
            "name": {"type": "string"},
            "age": {"type": "integer", "minimum": 0}
        },
        "required": ["name", "age"]
    })";
    
    // JSON data to validate
    const char *json_str = "{\"name\": \"John\", \"age\": 30}";
    
    jso_value schema_doc, json_doc;
    jso_schema *schema;
    jso_parser_options options;
    
    jso_parser_options_init(&options);
    
    // Parse schema
    if (jso_parse_cstr(schema_str, strlen(schema_str), &options, &schema_doc) == JSO_SUCCESS) {
        // Create and parse schema
        schema = jso_schema_alloc();
        if (jso_schema_parse(schema, &schema_doc) == JSO_SUCCESS) {
            // Parse JSON data
            if (jso_parse_cstr(json_str, strlen(json_str), &options, &json_doc) == JSO_SUCCESS) {
                // Validate
                jso_virt_value *virt_value = jso_value_to_virt_value(&json_doc);
                jso_schema_validation_result result = jso_schema_validate(schema, virt_value);
                
                if (result == JSO_SCHEMA_VALIDATION_VALID) {
                    printf("Validation successful!\n");
                } else {
                    printf("Validation failed!\n");
                }
                
                jso_value_free(&json_doc);
            }
        }
        jso_schema_free(schema);
        jso_value_free(&schema_doc);
    }
    
    return 0;
}
```

### JSON Encoding

```c
#include "jso.h"

int main() {
    jso_value json_value;
    jso_encoder_options enc_options;
    jso_io *output;
    
    // Parse some JSON first
    const char *json_str = "{\"name\": \"John\", \"age\": 30}";
    jso_parser_options parse_options;
    jso_parser_options_init(&parse_options);
    
    if (jso_parse_cstr(json_str, strlen(json_str), &parse_options, &json_value) == JSO_SUCCESS) {
        // Set up encoding options
        enc_options.max_depth = JSO_ENCODER_DEPTH_UNLIMITED;
        enc_options.pretty = JSO_TRUE;  // Pretty print
        
        // Create output stream (stdout in this case)
        output = jso_io_file_open_stream(stdout);
        
        // Encode to output
        jso_encode(&json_value, output, &enc_options);
        
        // Clean up
        jso_value_free(&json_value);
        JSO_IO_FREE(output);
    }
    
    return 0;
}
```

## JSON Schema Support

The library supports JSON Schema validation with the following features:

### Supported Schema Versions
- JSON Schema Draft 4
- JSON Schema Draft 6

### Supported Keywords
- **Type validation**: `type`, `enum`, `const`
- **String validation**: `minLength`, `maxLength`, `pattern`
- **Numeric validation**: `minimum`, `maximum`, `exclusiveMinimum`, `exclusiveMaximum`, `multipleOf`
- **Array validation**: `items`, `additionalItems`, `minItems`, `maxItems`, `uniqueItems`, `contains`
- **Object validation**: `properties`, `additionalProperties`, `required`, `minProperties`, `maxProperties`, `patternProperties`, `dependencies`, `propertyNames`
- **Composition**: `allOf`, `anyOf`, `oneOf`, `not`
- **References**: `$ref`, `definitions`
- **Metadata**: `title`, `description`, `default`

### Schema Usage Example

```c
jso_schema_options schema_opts;
jso_schema_options_init(&schema_opts);
schema_opts.default_version = JSO_SCHEMA_VERSION_DRAFT_06;

jso_schema *schema = jso_schema_alloc();
if (jso_schema_parse_ex(schema, &schema_doc, &schema_opts) == JSO_SUCCESS) {
    // Schema ready for validation
}
```

## JSON Pointer

The library includes RFC 6901 compliant JSON Pointer support:

```c
#include "jso_pointer.h"

// Create a pointer
jso_string *pointer_str = jso_string_create("/name");
jso_pointer *jp = jso_pointer_create(pointer_str);

// Resolve pointer in document
jso_value *referenced_value;
if (jso_pointer_resolve(jp, &document, &referenced_value) == JSO_SUCCESS) {
    // Use referenced_value
}

jso_pointer_free(jp);
```

## Command Line Interface

The library includes a command-line tool for JSON processing:

```bash
# Basic JSON validation and pretty printing
jso input.json

# Validate against a schema
jso --schema schema.json input.json

# Different output formats
jso --output-type minimal input.json
jso --output-type pretty input.json
jso --output-type debug input.json

# Set maximum parsing depth
jso --depth 100 input.json
```

### CLI Options

| Option | Short | Description |
|--------|-------|-------------|
| `--depth` | `-d` | Maximum allowed object nesting depth |
| `--help` | `-h` | Show help text |
| `--output-type` | `-o` | Output type: minimal, pretty, or debug |
| `--schema` | `-s` | JSON Schema file for validation |

## API Reference

### Core Types

- `jso_value`: Main value type that can hold any JSON value
- `jso_array`: JSON array type
- `jso_object`: JSON object type
- `jso_string`: String type
- `jso_schema`: JSON Schema type
- `jso_pointer`: JSON Pointer type

### Value Types

```c
typedef enum {
    JSO_TYPE_ERROR = 0,
    JSO_TYPE_NULL = 1,
    JSO_TYPE_BOOL = 2,
    JSO_TYPE_INT = 3,
    JSO_TYPE_DOUBLE = 4,
    JSO_TYPE_STRING = 5,
    JSO_TYPE_ARRAY = 6,
    JSO_TYPE_OBJECT = 7,
    JSO_TYPE_POINTER = 8,
    JSO_TYPE_SCHEMA_VALUE = 9,
} jso_value_type;
```

### Return Codes

```c
typedef enum {
    JSO_SUCCESS = 0,
    JSO_FAILURE = -1
} jso_rc;
```

### Error Handling

The library provides detailed error information:

```c
jso_value result;
if (jso_parse_cstr(json_str, len, &options, &result) == JSO_FAILURE) {
    jso_error_type error_type = jso_value_get_error_type(&result);
    const char *error_desc = jso_value_get_error_description(&result);
    printf("Error: %s\n", error_desc);
}
```

## Building

The library follows standard C compilation practices. Include the headers you need for your specific use case:

```c
#include "jso.h"              // Core library - always needed
#include "jso_parser.h"       // For JSON parsing functionality
#include "jso_encoder.h"      // For JSON encoding/output
#include "jso_schema.h"       // For JSON Schema validation
#include "jso_pointer.h"      // For JSON Pointer support
#include "jso_cli.h"          // For command-line interface features
// Your code here
```

For basic JSON parsing and encoding, you typically need:
```c
#include "jso.h"
#include "jso_parser.h"
#include "jso_encoder.h"
```

For schema validation, add:
```c
#include "jso_schema.h"
```

Compile with:
```bash
gcc -o myapp myapp.c -ljso
```

## Memory Management

The library uses reference counting and proper cleanup functions:

- Always call `jso_value_free()` for parsed values
- Use `jso_schema_free()` for schemas
- Use `jso_pointer_free()` for pointers
- IO streams should be freed with `JSO_IO_FREE()`

## Thread Safety

The library is designed to be thread-safe for read operations on immutable data structures. However, parsing and modification operations should be synchronized if used across multiple threads.

## Advanced Usage

### Embedding and Virtual API

For advanced integration scenarios, JSO provides a virtual API layer that allows embedders to override the default implementation. This is particularly useful when integrating JSO into existing systems that have their own data structures or memory management schemes.

The virtual API is defined in `jso_virt.h` and by default maps directly to the standard JSO types:

```c
#define jso_virt_value jso_value
#define jso_virt_string jso_string
#define jso_virt_array jso_array
#define jso_virt_object jso_object
```

To override this behavior, you can provide your own implementations of the virtual functions and redefine `JSO_VIRT_IS_JSO` to 0. This allows you to:

- Use custom data structures for JSON values
- Implement custom memory management
- Integrate with existing object systems
- Provide specialized validation or processing logic

Refer to `jso_virt.h` for the complete virtual API interface that can be overridden.
