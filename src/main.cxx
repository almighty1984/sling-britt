import app;
import app.config;
import types;

int main() {
    app::config::load("main.cfg");
    app::App app(app::config::window_size().x, app::config::window_size().y, app::config::scale(), "Sling-Britt");

    app.add_state(app::config::start_state());
    app.add_state(state::Type::overlay);    
    app.run();
    return 0;
}