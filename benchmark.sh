
echo "Creating big single file..."
cp big.txt big2.txt
{ for i in {1..100}; do cat big.txt >> big2.txt ; done } > /dev/null

cmd=( "grep" "ag" "gg" ) 
opt=( "-rn" "" "" ) 
pattern="copyright"
target="big2.txt"
drop_everything="/dev/null 2>&1 > /dev/null"
result=()

#echo "Profiling commands searching single large file..."
#for i in "${!cmd[@]}"; 
#do
#    # echo "${cmd[$i]}"
#    # echo "Warming read cache...";
#    ${cmd[$i]} ${opt[$i]} $pattern $target > /dev/null 2>&1 > /dev/null
#    # echo "Profiling..."
#    result[$i]=$( ( perf stat ${cmd[i]} ${opt[$i]} $pattern $target > /dev/null ) 2>&1 | grep "seconds time elapsed" | awk '{print $1}' )
#    # echo ${result[$i]}
#done
#
#for i in "${!result[@]}"; 
#do
#    # echo ${cmd[$i]} # --> ${result[$i]}
#    # echo ${result[$i]}
#    echo ${cmd[$i]}$'\t'${result[$i]}
#done

rm big2.txt
pushd ../linux-4.7.6

cmd=( "grep" "ag" "gg" ) 
opt=( "-rn --include=*.c --include=*.h" "--cc" "" ) 
pattern="copyright"
result2=()

echo "Profiling commands searching large repository"
for i in "${!cmd[@]}"; 
do
    # echo "${cmd[$i]}" "${opt[$i]}" "$pattern"
    echo "Warming read cache...";
    # ${cmd[$i]} ${opt[$i]} $pattern > /dev/null 2>&1 > /dev/null
    echo "Profiling..."
    result2[$i]=$( ( perf stat ${cmd[$i]} ${opt[$i]} $pattern > /dev/null ) 2>&1 | grep "seconds time" | awk '{print $1}')
    echo ${result2[$i]}
done

for i in "${!result2[@]}"; 
do
    # echo ${cmd[$i]} # --> ${result2[$i]}
    # echo ${result2[$i]}
    echo ${cmd[$i]}$'\t'${result2[$i]}
done

popd

#echo "gg single file"
#echo "warming read cache..."
#{ for i in {1..2}; do gg copyright big2.txt > /dev/null ; done } 2>&1 > /dev/null
#echo "benchmark result"
#{ time for i in {1..5}; do gg copyright big2.txt > /dev/null ; done } 2>&1 | grep real
#
#echo "grep -rn multiple file"
#echo "warming read cache..."
#{ for i in {1..2}; do grep -rn copyright big2.txt > /dev/null ; done } 2>&1 > /dev/null
#echo "benchmark result"
#{ time for i in {1..5}; do grep -rn copyright big2.txt > /dev/null ; done } 2>&1 | grep real
#
#echo "Remove big single file..."
## rm big2.txt
#
#pushd ../linux-4.7.6
#
#echo "ag --cc multiple file"
#echo "warming read cache..."
#{ for i in {1..2}; do ag --cc late_initcall_sync > /dev/null ; done } 2>&1 > /dev/null
#echo "benchmark result"
#{ time for i in {1..5}; do ag --cc late_initcall_sync > /dev/null ; done } 2>&1 | grep real
#
#echo "gg multiple file"
#echo "warming read cache..."
#{ for i in {1..2}; do gg late_initcall_sync > /dev/null ; done } 2>&1 > /dev/null
#echo "benchmark result"
#{ time for i in {1..5}; do gg late_initcall_sync > /dev/null ; done } 2>&1 | grep real
#
#echo "grep -rn multiple file"
#echo "warming read cache..."
#{ for i in {1..2}; do find -name '*.h' -o -name '*.c' | xargs grep -rn big2.txt > /dev/null ; done } 2>&1 > /dev/null
#echo "benchmark result"
#{ time for i in {1..5}; do find -name '*.h' -o -name '*.c' | xargs grep -rn big2.txt > /dev/null ; done } 2>&1 | grep real
#
#popd
