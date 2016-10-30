
echo "Creating big single file..."
cp big.txt big2.txt
{ for i in {1..100}; do cat big.txt >> big2.txt ; done } > /dev/null

cmd=( "grep" "ag" "gg" ) 
opt=( "-rn" "" "" ) 
pattern="copyright"
target="big2.txt"
drop_everything="/dev/null 2>&1 > /dev/null"
result=()

echo "Profiling commands searching single large file..."
for i in "${!cmd[@]}"; 
do
    # echo "${cmd[$i]}"
    # echo "Warming read cache...";
    ${cmd[$i]} ${opt[$i]} $pattern $target > /dev/null 2>&1 > /dev/null
    # echo "Profiling..."
    result[$i]=$( ( perf stat ${cmd[i]} ${opt[$i]} $pattern $target > /dev/null ) 2>&1 | grep "seconds time elapsed" | awk '{print $1}' )
    # echo ${result[$i]}
done

rm big2.txt
pushd ../linux-4.7.6

opt=( "-rn --include=*.c --include=*.h" "--cc" "" ) 
pattern="virt_to_page"
time=()
cpu=()

echo "Profiling commands searching large repository"
for i in "${!cmd[@]}"; 
do
    # echo "${cmd[$i]}" "${opt[$i]}" "$pattern"
    echo "Warming read cache...";
    # ${cmd[$i]} ${opt[$i]} $pattern > /dev/null 2>&1 > /dev/null
    echo "Profiling..."
    ( perf stat ${cmd[$i]} ${opt[$i]} $pattern > /dev/null ) 2>&1 | tee tmp 

    cpu[$i]=$(cat tmp | grep "CPU" | awk '{print $5}')
    time[$i]=$(cat tmp | grep "seconds time" | awk '{print $1}')

    # echo "${time[$i]}"
    # echo ${cpu[$i]}
done

popd


echo "Single file profile result:"

for i in "${!cmd[@]}"; 
do
    echo ${cmd[$i]}$'\t'${result[$i]}
done

echo "Multifile file profile result:"

for i in "${!cmd[@]}"; 
do
    echo ${cmd[$i]}$'\t'${time[$i]}$'\t'${cpu[$i]}
done

