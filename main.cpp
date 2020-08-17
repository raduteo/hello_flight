#include <iostream>
#include <arrow/flight/server.h>

int main() {
    arrow::flight::FlightServerBase server;
    server.Serve();
    return 0;
}
