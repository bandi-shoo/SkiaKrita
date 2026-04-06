/// @file command.cpp
/// @brief CommandHistory implementation.

#include "core/command.h"

#include <vector>

namespace skiakrita::core {

// ── Pimpl ────────────────────────────────────────────────────────
struct CommandHistory::Impl {
    std::vector<std::unique_ptr<Command>> undoStack;
    std::vector<std::unique_ptr<Command>> redoStack;
};

// ── Lifetime ─────────────────────────────────────────────────────
CommandHistory::CommandHistory()
    : m_impl(std::make_unique<Impl>()) {}

CommandHistory::~CommandHistory()                                   = default;
CommandHistory::CommandHistory(CommandHistory&&) noexcept           = default;
CommandHistory& CommandHistory::operator=(CommandHistory&&) noexcept = default;

// ── Operations ───────────────────────────────────────────────────
void CommandHistory::execute(std::unique_ptr<Command> cmd) {
    cmd->execute();
    m_impl->undoStack.push_back(std::move(cmd));
    m_impl->redoStack.clear();
}

bool CommandHistory::canUndo() const { return !m_impl->undoStack.empty(); }
bool CommandHistory::canRedo() const { return !m_impl->redoStack.empty(); }

void CommandHistory::undo() {
    if (!canUndo()) return;
    auto cmd = std::move(m_impl->undoStack.back());
    m_impl->undoStack.pop_back();
    cmd->undo();
    m_impl->redoStack.push_back(std::move(cmd));
}

void CommandHistory::redo() {
    if (!canRedo()) return;
    auto cmd = std::move(m_impl->redoStack.back());
    m_impl->redoStack.pop_back();
    cmd->execute();
    m_impl->undoStack.push_back(std::move(cmd));
}

void CommandHistory::clear() {
    m_impl->undoStack.clear();
    m_impl->redoStack.clear();
}

std::size_t CommandHistory::undoCount() const { return m_impl->undoStack.size(); }
std::size_t CommandHistory::redoCount() const { return m_impl->redoStack.size(); }

} // namespace skiakrita::core
