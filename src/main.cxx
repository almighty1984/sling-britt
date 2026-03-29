import app;
import app.config;
import types;
import std;

int main() {
    app::config::load("main.cfg");
    app::App app(app::config::extent().x, app::config::extent().y, app::config::scale(), "Sling-Britt");

    app.add_sheet(app::config::start_sheet());
    app.add_sheet(sheet::Type::overlay);    
    app.run();
    return 0;
}