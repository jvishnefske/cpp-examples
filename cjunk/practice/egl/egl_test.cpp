#include <iostream>
#include <memory>
#include <stdexcept>

// Mock EGL types and constants for demonstration
using EGLDisplay = void*;
using EGLSurface = void*;
using EGLContext = void*;
using EGLConfig = void*;
using EGLNativeWindowType = unsigned long;
using EGLint = int;

constexpr EGLDisplay EGL_NO_DISPLAY = nullptr;
constexpr int EGL_FALSE = 0;
constexpr int EGL_TRUE = 1;
constexpr EGLDisplay EGL_DEFAULT_DISPLAY = nullptr;

// Mock EGL functions for demonstration
EGLDisplay mock_eglGetDisplay(EGLDisplay) {
    return reinterpret_cast<EGLDisplay>(0x12345);
}

int mock_eglInitialize(EGLDisplay, EGLint*, EGLint*) {
    return EGL_TRUE;
}

void mock_eglTerminate(EGLDisplay) {
    // Mock termination
}

EGLSurface mock_eglCreateWindowSurface(EGLDisplay, EGLConfig, EGLNativeWindowType, const EGLint*) {
    return reinterpret_cast<EGLSurface>(0x54321);
}

void mock_eglDestroySurface(EGLDisplay, EGLSurface) {
    // Mock surface destruction
}

// RAII wrapper for EGL Display
class EGLDisplayWrapper {
public:
    EGLDisplayWrapper() : display_(mock_eglGetDisplay(EGL_DEFAULT_DISPLAY)) {
        if (display_ == EGL_NO_DISPLAY) {
            throw std::runtime_error("Failed to get EGL display");
        }
        
        if (mock_eglInitialize(display_, nullptr, nullptr) == EGL_FALSE) {
            throw std::runtime_error("Failed to initialize EGL");
        }
        
        std::cout << "EGL Display initialized successfully" << std::endl;
    }
    
    ~EGLDisplayWrapper() {
        if (display_ != EGL_NO_DISPLAY) {
            mock_eglTerminate(display_);
            std::cout << "EGL Display terminated" << std::endl;
        }
    }
    
    EGLDisplay get() const { return display_; }
    
    // Non-copyable
    EGLDisplayWrapper(const EGLDisplayWrapper&) = delete;
    EGLDisplayWrapper& operator=(const EGLDisplayWrapper&) = delete;
    
    // Movable
    EGLDisplayWrapper(EGLDisplayWrapper&& other) noexcept : display_(other.display_) {
        other.display_ = EGL_NO_DISPLAY;
    }
    
    EGLDisplayWrapper& operator=(EGLDisplayWrapper&& other) noexcept {
        if (this != &other) {
            if (display_ != EGL_NO_DISPLAY) {
                mock_eglTerminate(display_);
            }
            display_ = other.display_;
            other.display_ = EGL_NO_DISPLAY;
        }
        return *this;
    }

private:
    EGLDisplay display_;
};

// RAII wrapper for EGL Surface
class EGLSurfaceWrapper {
public:
    EGLSurfaceWrapper(EGLDisplay display, EGLConfig config, EGLNativeWindowType window) 
        : display_(display), surface_(nullptr) {
        
        surface_ = mock_eglCreateWindowSurface(display_, config, window, nullptr);
        if (!surface_) {
            throw std::runtime_error("Failed to create EGL surface");
        }
        
        std::cout << "EGL Surface created successfully" << std::endl;
    }
    
    ~EGLSurfaceWrapper() {
        if (surface_) {
            mock_eglDestroySurface(display_, surface_);
            std::cout << "EGL Surface destroyed" << std::endl;
        }
    }
    
    EGLSurface get() const { return surface_; }
    
    // Non-copyable
    EGLSurfaceWrapper(const EGLSurfaceWrapper&) = delete;
    EGLSurfaceWrapper& operator=(const EGLSurfaceWrapper&) = delete;
    
    // Movable
    EGLSurfaceWrapper(EGLSurfaceWrapper&& other) noexcept 
        : display_(other.display_), surface_(other.surface_) {
        other.surface_ = nullptr;
    }
    
    EGLSurfaceWrapper& operator=(EGLSurfaceWrapper&& other) noexcept {
        if (this != &other) {
            if (surface_) {
                mock_eglDestroySurface(display_, surface_);
            }
            display_ = other.display_;
            surface_ = other.surface_;
            other.surface_ = nullptr;
        }
        return *this;
    }

private:
    EGLDisplay display_;
    EGLSurface surface_;
};

// Simple EGL Manager class
class EGLManager {
public:
    EGLManager() : display_() {
        std::cout << "EGL Manager initialized" << std::endl;
    }
    
    std::unique_ptr<EGLSurfaceWrapper> createSurface(EGLNativeWindowType window) {
        // Mock config for demonstration
        EGLConfig config = reinterpret_cast<EGLConfig>(0x67890);
        
        return std::make_unique<EGLSurfaceWrapper>(display_.get(), config, window);
    }
    
    void renderFrame() {
        std::cout << "Rendering frame..." << std::endl;
        // Mock rendering operations
        std::cout << "- Clear color buffer" << std::endl;
        std::cout << "- Draw primitives" << std::endl;
        std::cout << "- Swap buffers" << std::endl;
    }
    
    EGLDisplay getDisplay() const {
        return display_.get();
    }

private:
    EGLDisplayWrapper display_;
};

// Demo function
void demo_egl_usage() {
    std::cout << "=== EGL Demo ===" << std::endl;
    
    try {
        // Create EGL manager
        EGLManager manager;
        
        // Mock window handle
        EGLNativeWindowType window = 12345;
        
        // Create surface
        auto surface = manager.createSurface(window);
        
        // Simulate rendering loop
        for (int frame = 0; frame < 3; ++frame) {
            std::cout << "\nFrame " << (frame + 1) << ":" << std::endl;
            manager.renderFrame();
        }
        
        std::cout << "\nEGL demo completed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "EGL Error: " << e.what() << std::endl;
        throw;
    }
}

void demo_raii_safety() {
    std::cout << "\n=== RAII Safety Demo ===" << std::endl;
    
    try {
        EGLDisplayWrapper display;
        std::cout << "Display created in scope" << std::endl;
        
        {
            // Mock config and window
            EGLConfig config = reinterpret_cast<EGLConfig>(0x11111);
            EGLNativeWindowType window = 54321;
            
            EGLSurfaceWrapper surface(display.get(), config, window);
            std::cout << "Surface created in inner scope" << std::endl;
            
            // Surface will be automatically destroyed when leaving this scope
        }
        
        std::cout << "Back in outer scope, surface destroyed" << std::endl;
        
        // Display will be automatically destroyed when leaving this scope
    } catch (const std::exception& e) {
        std::cerr << "RAII Error: " << e.what() << std::endl;
    }
    
    std::cout << "RAII demo completed" << std::endl;
}

int main() {
    std::cout << "=== EGL Test Application ===" << std::endl;
    
    try {
        demo_egl_usage();
        demo_raii_safety();
        
        std::cout << "\n=== All tests passed ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}