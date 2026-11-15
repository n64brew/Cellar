set -e

# build site generator itself
cd generator-cli
make -j8
cd ..

# copy assets outside of repo
rm -rf build
mkdir -p build

cp -r authors build
cp -r genre build
cp -r roms build
cp -r tags build
cp -r templates build

cp -r *.html build
cp -r *.ico build
cp -r *.css build
cp -r *.js build
cp -r *.png build
cp -r *.gif build

find build/ -type f -name '*.z64' -delete
find build/ -type f -name '*.n64' -delete
find build/ -type f -name '*.v64' -delete
find build/ -type f -name '*.zip' -delete

# build full website
cd build
../generator-cli/build/cellar-generator all

