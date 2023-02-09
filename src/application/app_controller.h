#include <functional>
#include <memory>
#include <string>
#include <string_view>

#include <ftxui/component/component.hpp>

#include <weechess/board.h>
#include <weechess/game_state.h>

class AppController {
    public:

        struct PastMove {
            weechess::Piece piece;
            weechess::Location location;
        };

        struct CommandOutput {
            enum class Type {
                Command,
                Error,
                Info,
            };

            std::string text;
            Type type;
        };

        struct State {
            weechess::GameState game_state;
            std::vector<PastMove> move_history;
            std::vector<CommandOutput> command_output;

            void push_command_error(std::string_view);
            void push_command_info(std::string_view);
        };

        class Delegate {
            public:
                virtual void on_should_redraw(AppController&) = 0;
                virtual void on_execute_command(AppController&, std::string_view command) = 0;
        };

        AppController();

        const ftxui::Component& renderer() const;
        void set_delegate(std::weak_ptr<Delegate> delegate);

        const State& state() const;
        void update_state(std::function<bool(State&)>);
 
        virtual ~AppController() = default;
    private:

        class CommandInput {
        private:
            std::string m_text;
            ftxui::Component m_input;

        public:
            CommandInput();

            std::string& text();
            const std::string& text() const;
            const ftxui::Component& renderer() const;
        };

        struct ViewState {

            enum class Focus {
                ChessWindow,
                CommandWindow,    
            };

            Focus focus;

            // Command window stuff
            CommandInput command_input;

            // Chess board stuff
            weechess::Location highlighted_location { 0 };
            std::optional<weechess::Location> selected_location { };

            ftxui::Component component_in_focus() const;

            ViewState();
        };

        State m_state;
        ViewState m_view_state;

        ftxui::Component m_renderer { };
        std::weak_ptr<Delegate> m_delegate { };

        ftxui::Element render();
};
