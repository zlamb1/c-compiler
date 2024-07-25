#!/bin/bash

no_path() {
    echo ${1##*/}  
}

no_ext() {
    echo ${1%%.*}
}

run_correct() {
    gcc -w "$1"
    ./a.out >/dev/null
    echo $?
}

run_custom() {
    output=$(eval "$1 $2")
    echo "$output"
    local no_ext=$(no_ext "$filename")
    temp=$(eval "$no_ext")
    exit_code=$?
    rm "$no_ext" >/dev/null 2>&1
    rm "$no_ext.s" >/dev/null 2>&1
    return $exit_code
}

absolute_dir=$(dirname "$(realpath $0)")
failed=false

# test valid examples
for filename in `find $absolute_dir -type f -name "*.c" -path "$absolute_dir/stage_$2/valid/*" -not -path "*/valid_multifile/*" 2>/dev/null`; do
    [ -f "$filename" ] || break;
    no_ext=$(no_ext "$filename")
    formatted_filename=$(no_path "$no_ext")
    # run gcc
    exit_code=$(run_correct "$filename")
    rm a.out >/dev/null 2>&1
    # run custom compiler
    compiler_output=$(run_custom $1 "${filename}")
    custom_exit_code=$?
    if [ "$exit_code" != "$custom_exit_code" ]; then
        echo ""
        echo "Test failed for $formatted_filename"
        echo "Excepted output: $exit_code"
        echo "Actual output: $custom_exit_code"
        echo "Compiler output: $compiler_output"
        echo ""
        failed=true
    fi
done

# test invalid examples
for filename in `find $absolute_dir -type f -name "*.c"  -path "$absolute_dir/stage_$2/invalid/*" -not -path "*/valid_multifile/*" 2>/dev/null`; do
    [ -f "$filename" ] || break;
    no_ext=$(no_ext "$filename")
    formatted_filename=$(no_path "$no_ext")
    compiler_output=$(eval "$1 $filename")
    if [ -f "$no_ext" ]; then
        echo ""
        echo "Test failed for $formatted_filename"
        echo "Compiler output: $compiler_output"
        echo ""
        rm "$no_ext" >/dev/null 2>&1
        failed=true
    fi
done

if [ "$failed" = true ]; then
    exit 1
fi
exit 0