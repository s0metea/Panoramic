
#include <stdio.h>
#include <string.h>
#include <thread>
#include "mongoose.h"
#include "../include/PanoramaMaker.h"

int value = 0;
int *action = &value;
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
                "<img id=\"left_img\" src=\"img/0.jpg\" alt=\"Left part\">\n"
                "<img id=\"right_img\" src=\"img/1.jpg\" alt=\"Right part\">\n"
                "<p><img id=\"center_img\" src=\"http://192.168.2.112:8080/?action=stream\" alt=\"Result of stitching\"></p>\n"
                "<p><img id=\"matches_img\" src=\"img/matches.jpg\" alt=\"Matches between left and right part\"></p>\n"
                "    <form action=\"http://192.168.2.112:80/handle_post_request\" method=\"post\">\n"
                "        <button type=\"submit\" name=\"input\" value=\"1\">Start</button>\n"
                "        <button type=\"submit\" name=\"input\" value=\"2\">Stop</button>\n"
                "<br>"
                "        <button type=\"submit\" name=\"input\" value=\"3\">Rebuild homography</button>\n"
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

static int handler(struct mg_connection *conn) {
    char var1[500];

    if (strcmp(conn->uri, "/handle_post_request") == 0) {
        mg_get_var(conn, "input", var1, sizeof(var1));
        *action = (int) *var1;
    }
    mg_send_data(conn, html_form, strlen(html_form));

    return 1;
}

void streamingStart() {
    string streamCommand = "mjpg_streamer -i  \"/root/data/Panoramic/plugins/input_file.so -f /root/data/Panoramic/img -n result.jpg\" -o \"/root/data/Panoramic/plugins/output_http.so -w ./www\"";
    int status = system(streamCommand.data());
}

int main(void) {
    struct mg_server *server = mg_create_server(NULL);
    mg_set_option(server, "listening_port", "80");
    mg_add_uri_handler(server, "/", handler);
    printf("Starting web interface on port %s\n", mg_get_option(server, "listening_port"));
    printf("Starting image streaming...");
    std::thread streaming(streamingStart);

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