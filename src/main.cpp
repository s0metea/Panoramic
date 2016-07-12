
#include <stdio.h>
#include <string.h>

#include "mongoose.h"
#include "../include/PanoramaMaker.h"

int value = 0;
int *action = &value;
string ip;
static const char *html_form =
        "<!DOCTYPE HTML>\n"
                "\n"
                "<html>\n"
                "\n"
                "<head>\n"
                "    <meta charset=\"UTF-8\" Cache-Control=\"no-cache\">\n"
                "    <title>Panoramic — Realtime panorama stitching</title>\n"
                "<body>\n"
                "\n"
                "<div style=\"text-align: center;\">\n"
                "<p><img id=\"center_img\" src=\"http://127.0.0.1:8080/?action=stream\" alt=\"Result of stitching\"></p>\n"
                "    <form action=\"http://127.0.0.1:9090/request\" method=\"post\">\n"
                "<br>"
                "        <button type=\"submit\" name=\"input\" value=\"3\">Rebuild homography</button>\n"
                "        <button type=\"submit\" name=\"input\" value=\"5\">Show FPS</button>\n"
                "<br>"
                "        <button type=\"submit\" name=\"input\" value=\"4\">Shutdown server</button>\n"
                "    </form>\n"
                "\n"
                "<footer>\n"
                "    <p>sometea1@gmail.com</p>\n"
                "    <p>2016</p>\n"
                "</footer>\n"
                "\n"
                "</div>\n"
                "</body>\n"
                "\n"
                "</html> ";

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

static int handler(struct mg_connection *conn) {
    char var1[500];

    if (strcmp(conn->uri, "/request") == 0) {
        mg_get_var(conn, "input", var1, sizeof(var1));
        *action = (int) *var1;
        cout << "Request with id: " << *action << endl;
    }
    mg_send_data(conn, html_form, (int) strlen(html_form));

    return 1;
}

int main(void) {
    struct mg_server *server = mg_create_server(NULL);
    string html(html_form);
    cout << "Enter ip of the server:" << endl;
    cin >> ip;
    //Fast set for debug
    if(ip == "0")
    {
        ip = "127.0.0.1";
    }
    if(ip == "1") {
        ip = "192.168.2.112";
    }
    replace(html, "127.0.0.1", ip);
    replace(html, "127.0.0.1", ip);
    html_form = html.data();
    cout << html_form;
    mg_set_option(server, "listening_port", "9090");
    mg_add_uri_handler(server, "/", handler);
    printf("Starting web interface on port %s\n", mg_get_option(server, "listening_port"));

    int firstCamera, secondCamera;
    int frameWidth, frameHeight;
    cout << "Hello, enter id of first camera: " << endl;
    cin >> firstCamera;
    cout << "Ok, first camera will be initialize with id " << firstCamera << "." << endl;
    cout << "Now you need to tell me id of the second camera:" << endl;
    cin >> secondCamera;
    cout << "Ok, second camera will be initialize with id " << secondCamera << "." << endl;

    cout << "Please enter width of the frame: " << endl;
    cin >> frameWidth;
    cout << "And height of the frame:" << endl;
    cin >> frameHeight;
    cout << "Trying to apply parameters..." << endl;

    vector<int> camerasID;
    camerasID.push_back(firstCamera);
    camerasID.push_back(secondCamera);
    PanoramaMaker pm(camerasID, frameWidth, frameHeight);
    cout << "Ok, we can start just now. Enjoy your realtime panorama stitching!" << endl;
    pm.start(server, action);
    return 0;
}