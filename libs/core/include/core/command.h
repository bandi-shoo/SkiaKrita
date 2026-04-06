#pragma once
/// @file command.h
/// @brief Command pattern with undo/redo history.

#include <cstddef>
#include <memory>
#include <string>

namespace skiakrita::core {

/// Abstract command – every undoable operation inherits from this.
class Command {
public:
    virtual ~Command() = default;

    virtual void execute() = 0;
    virtual void undo()    = 0;

    [[nodiscard]] virtual std::string description() const = 0;
};

/// Linear undo/redo history backed by two stacks.
class CommandHistory {
public:
    CommandHistory();
    ~CommandHistory();

    // Movable, non-copyable.
    CommandHistory(CommandHistory&&) noexcept;
    CommandHistory& operator=(CommandHistory&&) noexcept;
    CommandHistory(const CommandHistory&)            = delete;
    CommandHistory& operator=(const CommandHistory&) = delete;

    /// Execute a command and push it onto the undo stack.
    /// Clears the redo stack.
    void execute(std::unique_ptr<Command> cmd);

    [[nodiscard]] bool canUndo() const;
    [[nodiscard]] bool canRedo() const;

    void undo();
    void redo();
    void clear();

    [[nodiscard]] std::size_t undoCount() const;
    [[nodiscard]] std::size_t redoCount() const;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace skiakrita::core
