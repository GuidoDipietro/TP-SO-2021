cd ~
mkdir so-libraries
cd so-libraries

# Instalamos la libreria de gui

sudo apt-get install -y libncurses5-dev
git clone https://github.com/sisoputnfrba/so-nivel-gui-library/
cd so-nivel-gui-library
make install
cd ..

# Instalamos las commons

git clone https://github.com/sisoputnfrba/so-commons-library.git
cd so-commons-library
sudo make install