#include <fstream>
#include "../include/mongoose.h"
#include "../include/json/json.h"
#include "../include/PanoramaMaker.h"


static const char *html_form =
                "<html>\n"
                "\n"
                "<head>\n"
                "    <meta charset=\"UTF-8\">\n"
                "    <title>Panoramic — Realtime panorama stitching</title>\n"
                "<body>\n"
                "\n"
                "   <div style=\"text-align: center;\">\n"
                "       <p><img style=\"width:85%\"id=\"center_img\" src=\"http://127.0.0.1:9090\" alt=\"Result of stitching\"></p>\n"
                "<br>"
                "        Refresh the page for homography rebuilding!"
                "\n"
                "<footer>\n"
                "    <p><a href =\"http://sometea.me\">Panoramic</a> — 2016</p>\n"
                "</footer>\n"
                "\n"
                "</div>\n"
                "</body>\n"
                "\n"
                "</html>\n";

vector<unsigned char> buf;
vector<int> param;
Mat warped;
PanoramaMaker *pm;

static void new_stream_connection_handler(struct mg_connection *conn, int ev, void *ev_data) {
    switch(ev) {
        case MG_EV_ACCEPT:
            mg_printf(conn,
                      "HTTP/1.0 200 OK\r\n"
                              "Cache-Control: no-cache\r\n"
                              "Connection: close\r\nContent-Type: multipart/x-mixed-replace; "
                              "boundary=--amazingboundary\r\n\r\n");
            break;
        default:
            mg_printf(conn,
                      "--amazingboundary\r\n"
                              "Content-Type: image/jpeg\r\n"
                              "Content-length: %d\r\n"
                              "\r\n", (int) buf.size());
            mg_send(conn, &buf[0], (int) buf.size());
            mg_printf(conn, "\r\n");
    }

}

static void remote_control_handler(struct mg_connection *conn, int ev, void *ev_data) {

    switch (ev) {
        case MG_EV_ACCEPT:
            mg_printf(conn, "HTTP/1.0 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Cache-Control: no-cache\r\n");
            pm->rebuildHomography();
            mg_printf(conn, html_form);
            break;
    }
}


int main(int argc, char* argv[]) {
    string configFilename = "config.json";
    if(argc > 1) {
        configFilename.clear();
        configFilename = argv[1];
    }

    Json::Value root;
    std::ifstream config_doc(configFilename, std::ifstream::binary);
    config_doc >> root;

    int frameWidth, frameHeight;

    frameWidth = root.get("w", 800).asInt();
    frameHeight = root.get("h", 600).asInt();

    int camerasAmount = root.get("number_of_cameras", 2).asInt();
    vector<int> camerasID;
    for(int i = 0; i < camerasAmount; i++) {
        camerasID.push_back(root["cameras"][i].get("id", i).asInt());
    }

    pm = new PanoramaMaker(camerasID, frameWidth, frameHeight);

    param.push_back(CV_IMWRITE_JPEG_QUALITY);
    param.push_back(40);

    struct mg_mgr mgr;
    string remote_control_port = root.get("remote_control_port", 8080).asString();
    string stream_port = root.get("stream_port", 9090).asString();

    //TODO: Add page handle.
    //string remote_control_page = root.get("remote_control_page", "control").asString();
    //string stream_page = root.get("stream_page", 8080).asString();

    string stream_path = stream_port;
    string remote_control_path = remote_control_port;

    mg_mgr_init(&mgr, NULL);  // Initialize event manager object
    mg_bind(&mgr, stream_path.c_str(), new_stream_connection_handler);  // Create listening connection and add it to the event manager
    mg_bind(&mgr, remote_control_path.c_str(), remote_control_handler);
    pm->initialize();
    cout << "Ok, we can start just now. Enjoy your realtime panorama stitching!" << endl;
    while(true) {
        //Getting new frame
        warped = pm->getWarped();
        imencode(".jpg", warped, buf, param);
        mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);
    cout << "Bye..." << endl;
    return 0;
}



