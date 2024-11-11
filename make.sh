if [ $# -eq 0 ]; then
    sudo rm /var/www/html/index.* 
    cd client/build
    make
    sudo cp index.* /var/www/html
    cd ../../server/build/
    make
    ./program
elif [ "$1" = "local" ]; then
    cd client/build
    make
    kill $(lsof -t -i:8080)
    python3 -m http.server 8080 &
    cd ../../server/build/
    make
    ./program
fi