sudo rm /var/www/html/index.* 
cd build
make
sudo cp index.* /var/www/html
