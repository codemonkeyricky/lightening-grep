
echo "Creating big single file..."
cp big.txt big2.txt
{ for i in {1..100}; do cat big.txt >> big2.txt ; done } > /dev/null

echo "ag single file"
echo "warming read cache..."
{ for i in {1..2}; do ag copyright big2.txt > /dev/null ; done } 2>&1 > /dev/null
echo "benchmark result"
{ time for i in {1..5}; do ag copyright big2.txt > /dev/null ; done } 2>&1 | grep real

echo "gg single file"
echo "warming read cache..."
{ for i in {1..2}; do gg copyright big2.txt > /dev/null ; done } 2>&1 > /dev/null
echo "benchmark result"
{ time for i in {1..5}; do gg copyright big2.txt > /dev/null ; done } 2>&1 | grep real

echo "grep -rn multiple file"
echo "warming read cache..."
{ for i in {1..2}; do grep -rn copyright big2.txt > /dev/null ; done } 2>&1 > /dev/null
echo "benchmark result"
{ time for i in {1..5}; do grep -rn copyright big2.txt > /dev/null ; done } 2>&1 | grep real

echo "Remove big single file..."
# rm big2.txt

pushd ../linux-4.7.6

echo "ag --cc multiple file"
echo "warming read cache..."
{ for i in {1..2}; do ag --cc late_initcall_sync > /dev/null ; done } 2>&1 > /dev/null
echo "benchmark result"
{ time for i in {1..5}; do ag --cc late_initcall_sync > /dev/null ; done } 2>&1 | grep real

echo "gg multiple file"
echo "warming read cache..."
{ for i in {1..2}; do gg late_initcall_sync > /dev/null ; done } 2>&1 > /dev/null
echo "benchmark result"
{ time for i in {1..5}; do gg late_initcall_sync > /dev/null ; done } 2>&1 | grep real

echo "grep -rn multiple file"
echo "warming read cache..."
{ for i in {1..2}; do find -name '*.h' -o -name '*.c' | xargs grep -rn big2.txt > /dev/null ; done } 2>&1 > /dev/null
echo "benchmark result"
{ time for i in {1..5}; do find -name '*.h' -o -name '*.c' | xargs grep -rn big2.txt > /dev/null ; done } 2>&1 | grep real

popd
