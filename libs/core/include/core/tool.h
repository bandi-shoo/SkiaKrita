#pragma once
/// @file tool.h
/// @brief Abstract tool interface – base for all interactive canvas tools.

#include <core/input_event.h>

#include <string>

namespace skiakrita::core {

class Document;

/// Base class for canvas tools (brush, eraser, selection, …).
class Tool {
public:
    virtual ~Tool() = default;

    virtual void onMousePress  (Document& doc, const MouseEvent& e) = 0;
    virtual void onMouseMove   (Document& doc, const MouseEvent& e) = 0;
    virtual void onMouseRelease(Document& doc, const MouseEvent& e) = 0;

    [[nodiscard]] virtual std::string name() const = 0;

    /// Convenience dispatcher.
    void handleEvent(Document& doc, const MouseEvent& e) {
        switch (e.type) {
            case MouseEvent::Type::Press:   onMousePress(doc, e);   break;
            case MouseEvent::Type::Move:    onMouseMove(doc, e);    break;
            case MouseEvent::Type::Release: onMouseRelease(doc, e); break;
        }
    }
};

} // namespace skiakrita::core
