#sudo apt-get install -y libncurses5-dev
original=$(pwd)
echo $original

# Instalamos las commons

cd ~
mkdir so-libraries
cd so-libraries

git clone https://github.com/sisoputnfrba/so-commons-library.git
cd so-commons-library
sudo make install
cd ..

# Instalamos la libreria de gui

git clone https://github.com/sisoputnfrba/so-nivel-gui-library/
cd so-nivel-gui-library
make install
cd ..

cd $original
make build
mkdir i_mongo_store/fs
mkdir i_mongo_store/fs/Files
mkdir i_mongo_store/fs/Files/Bitacoras

git clone https://github.com/sisoputnfrba/a-mongos-pruebas.git
mv a-mongos-pruebas/Finales/*.txt discordiador
rm -rf a-mongos-pruebas