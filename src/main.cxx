import app;
import config;
import state;

int main() {
    Config::load("main.cfg");
    App app(320, 180, Config::scale(), "Sling-Britt");
    app.add_state(Config::state_type());
    app.add_state(state::Type::overlay);    
    app.run();
    return 0;
}