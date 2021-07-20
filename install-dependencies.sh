#sudo apt-get install -y libncurses5-dev
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

