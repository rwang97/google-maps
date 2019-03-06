/* 
 * Copyright 2018 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <iostream>
#include <string>
#include "m1.h"
#include "m2.h"
#include "StreetsDatabaseAPI.h"
#include "m3.h"
#include "graphics.h"
#include "map_data.h"

using namespace std;

//Program exit codes
constexpr int SUCCESS_EXIT_CODE = 0;        //Everyting went OK
constexpr int ERROR_EXIT_CODE = 1;          //An error occured
constexpr int BAD_ARGUMENTS_EXIT_CODE = 2;  //Invalid command-line usage
//The default map to load if none is specified
std::string default_map_path = "/cad2/ece297s/public/maps/toronto_canada.streets.bin";

int main(int argc, char** argv) {

    string userin;
    string mapName;
    string map_path = default_map_path;
    
    if(argc == 1) {
        //Use a default map
        //map_path = default_map_path;
    } else if (argc == 2) {
        //Get the map from the command line
        //map_path = argv[1];
    } else {
        //Invalid arguments
        std::cerr << "Usage: " << argv[0] << " [map_file_path]\n";
        std::cerr << "  If no map_file_path is provided a default map is loaded.\n";
        return BAD_ARGUMENTS_EXIT_CODE;
    }
    
//    cout << "Please enter a valid city name" << endl;
//
//    //These maps are the ones available in public folder
//    cout << "Available maps are:\n Beijing, Cairo\n Cape Town, Hamilton\n "
//            "Golden Horseshoe, Hong Kong\n Iceland, Interlaken\n London, Moscow"
//            "\n New Delhi, New York\n Rio de Janeiro, Saint Helena\n Singapore, "
//            "Sidney\n Tehran, Tokyo\n Toronto" << endl;
//    cout <<"Map name: ";
//    while (getline(cin, userin)) {
//
//        if (cin.eof())
//            break;
//
//        else {
//            bool valid = true;
//            if (userin == "Cairo") {
//                mapName = "cairo_egypt";
//
//            } else if (userin == "Hamilton") {
//                mapName = "hamilton_canada";
//
//            } else if (userin == "Tokyo") {
//                mapName = "tokyo_japan";
//
//            } 
//            else if (userin == "Beijing") {
//                mapName = "beijing_china";
//            
//            } 
//            else if (userin == "Cape Town") {
//                mapName = "cape-town_south-africa";
//
//            } 
//            else if (userin == "Hong Kong") {
//                mapName = "hong-kong_china";
//
//            }else if (userin == "Iceland") {
//                mapName = "iceland";
//
//            }else if (userin == "Interlaken") {
//                mapName = "interlaken_switzerland";
//
//            }else if (userin == "New Delhi") {
//                mapName = "new-delhi_india";
//
//            }else if (userin == "Rio de Janeiro") {
//                mapName = "rio-de-janeiro_brazil";
//
//            }else if (userin == "Singapore") {
//                mapName = "singapore";
//
//            }else if (userin == "Sydney") {
//                mapName = "sydney_australia";
//
//            }else if (userin == "Tehran") {
//                mapName = "tehran_iran";
//
//            }
//            else if (userin == "London") {
//                mapName = "london_england";
//
//            }else if (userin == "Moscow") {
//                mapName = "moscow_russia";
//
//            }
//            else if (userin == "New York") {
//                mapName = "new-york_usa";
//
//            } else if (userin == "Saint Helena") {
//                mapName = "saint_helena";
//
//            } else if (userin == "Toronto") {
//                mapName = "toronto_canada";
//
//            } else if (userin == "Golden Horseshoe") {
//                mapName = "golden-horseshoe_canada";
//
//            } else {
//                cout << endl << "Invalid Map Name" << endl << endl;
//                valid = false;
//            }
//
//            if (valid) {
//
//                cout << "Please wait for ~3 seconds" << endl;
//
//                //concatenate the strings to form the map path, and use the map path to load
//                map_path = "/cad2/ece297s/public/maps/" + mapName + ".streets.bin";
//
                //load_map("/cad2/ece297s/public/maps/saint-helena.streets.bin");
                load_map(map_path);
//                                               
//                //finish loading, all data structures should be filled up, now draw the map
                draw_map();
//
//                //after drawing we close the map and free all data
                close_map();
//            }
//
//            cout << "Please enter a valid city name" << endl;
//            cout << "Available maps are:\n Beijing, Cairo\n Cape Town, Hamilton\n "
//            "Golden Horseshoe, Hong Kong\n Iceland, Interlaken\n London, Moscow"
//            "\n New Delhi, New York\n Rio de Janeiro, Saint Helena\n Singapore, "
//            "Sidney\n Tehran, Tokyo\n Toronto" << endl;
//            cout <<"Map name: ";
//        }
//    }

    return SUCCESS_EXIT_CODE;
}
