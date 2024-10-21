cd build
CC=icc CXX=icpc cmake -DCMAKE_BUILD_TYPE=RELEASE ..
make

rm fire_*.txt tree_*.txt temp_*.txt stats.txt
./fire

cd ../py
python3 draw_temperature.py
cd ..
