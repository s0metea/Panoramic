
#include <stdio.h>
#include <string.h>
#include "mongoose.h"
#include "PanoramaMaker.h"

int value = 0;
int *action = &value;
static const char *html_form =
        "<html><body>POST example."
                "<form method=\"POST\" action=\"/handle_post_request\">"
                "Input 1: <input type=\"text\" name=\"input_1\" /> <br/>"
                "Input 2: <input type=\"text\" name=\"input_2\" /> <br/>"
                "<input type=\"submit\" />"
                "</form></body></html>";

static int handler(struct mg_connection *conn) {
    char var1[500];

    if (strcmp(conn->uri, "/handle_post_request") == 0) {
        // User has submitted a form, show submitted data and a variable value
        // Parse form data. var1 and var2 are guaranteed to be NUL-terminated
        mg_get_var(conn, "input_1", var1, sizeof(var1));
        *action = (int) *var1;

    } else {
        // Show HTML form.
        mg_send_data(conn, html_form, strlen(html_form));
    }

    return 1;
}

int main(void) {

    struct mg_server *server = mg_create_server(NULL);
    mg_set_option(server, "listening_port", "8080");
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