#!/bin/zsh +x
OUTPUT_LIBNAME=$(dirname $0)
cd $(dirname $0)
OUTPUT_LIBNAME=$(basename $OUTPUT_LIBNAME)

echo $(pwd)

rm -rf ./generated
rm -rf ./lib
mkdir ./generated ./lib

for each_platform_target in 464 6128; do
    rm -rf ./generated
    mkdir ./generated
    
    for each_source in src/**/*.c; do
        echo "Compiling => [$each_source]"
        ../../build/sdcc/darwin/4.5.0/bin/sdcc -c -mz80 --Werror --disable-warning 59 --opt-code-speed --max-allocs-per-node 1000000 --allow-unsafe-read -DCPC_MODEL_${each_platform_target} -I ./inc -o ./generated/ $each_source || exit 1
    done

    echo "Compiled result:"
    for each_res in generated/*.rel; do
        echo "\t$each_res"
    done

    echo "Linking:"
    ../../build/sdcc/darwin/4.5.0/bin/sdar rc -o lib/${OUTPUT_LIBNAME}_${each_platform_target}.lib generated/**/*.rel || exit 1
done

echo "Generated lib(s):"
for each_res in lib/*.lib; do
    echo "\t$each_res"
done

