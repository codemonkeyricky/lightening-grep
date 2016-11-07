
echo "Creating big single file..."
cp big.txt big2.txt
{ for i in {1..50}; do cat big.txt >> big2.txt ; done } > /dev/null

cmd=( "grep" "ag" "gg" ) 
opt=( "-rn" "" "" ) 
pattern="copyright"
target="big2.txt"
drop_everything="/dev/null 2>&1 > /dev/null"
result=()
time1=()
cpu1=()

echo "Profiling commands searching single large file..."
for i in "${!cmd[@]}"; 
do
    echo "Command to profile:" "${cmd[$i]}" "${opt[$i]}" "$pattern" "$target"
    # echo "Warming read cache...";
    ${cmd[$i]} ${opt[$i]} $pattern $target > /dev/null 2>&1 > /dev/null
    # echo "Profiling..."
    ( perf stat ${cmd[i]} ${opt[$i]} $pattern $target > /dev/null ) > tmp 2>&1

    cpu1[$i]=$(cat tmp | grep "CPU" | awk '{print $5}')
    time1[$i]=$(cat tmp | grep "seconds time" | awk '{print $1}')

done

rm big2.txt
pushd ../linux

opt=( "-rn --include=*.c --include=*.h" "--cc" "--cc" ) 
pattern="virt_to_page"
time2=()
cpu2=()

echo "Profiling commands searching large repository"
for i in "${!cmd[@]}"; 
do
    echo "Command to profile:" "${cmd[$i]}" "${opt[$i]}" "$pattern"
    # echo "Warming read cache...";
    # ${cmd[$i]} ${opt[$i]} $pattern > /dev/null 2>&1 > /dev/null
    # echo "Profiling..."
    ( perf stat ${cmd[$i]} ${opt[$i]} $pattern > /dev/null ) > tmp 2>&1 

    cpu2[$i]=$(cat tmp | grep "CPU" | awk '{print $5}')
    time2[$i]=$(cat tmp | grep "seconds time" | awk '{print $1}')
done

popd


echo "Single file profile result:"

for i in "${!cmd[@]}"; 
do
    echo ${cmd[$i]}$'\t'${time1[$i]}$'\t'${cpu1[$i]}
done

echo "Multifile file profile result:"

for i in "${!cmd[@]}"; 
do
    norm=0
    if hash calc > /dev/null; then 
        norm=$( calc ${cpu2[$i]}*${time2[$i]} ) 
    fi

    echo ${cmd[$i]}$'\t'"Time: "${time2[$i]}$'\t'"CPUs Used: "${cpu2[$i]}$'\t'"Normalized: "${norm}
done

