#ifndef DPJ_DRAWABLE_HH_
#define DPJ_DRAWABLE_HH_

#include <string>
#include <vector>

#include "renderer.hh"
#include "transform.hh"

namespace dpj
{
  namespace gl
  {    
    // draw(T const& x, renderer& renderer)
    //
    // This method tells how to draw an arbitrary data type.
    //
    // - to be specialized.
    //
    template<typename T>
    void draw(T const&, renderer const&) { }
    
    
    // This type is constructed from an arbitrary type.
    //
    // - has value semantics
    // - dispatches to runtime type
    //
    class drawable_t
    {
    public:
      
      // Construction
      //
      template<typename T>
      drawable_t(T x) : self(new model<T>{std::move(x)}) { }
      drawable_t(drawable_t const& x) : self(x.self->copy()) { }
      drawable_t(drawable_t&&) noexcept = default;
      
      
      // Assignment
      //
      drawable_t& operator=(drawable_t&&) noexcept = default;
      drawable_t& operator=(drawable_t const& x)
      { drawable_t tmp{x}; *this = std::move(tmp); return *this; }
      
      
      // void draw(drawable_t const&, renderer&)
      //
      // This method draws this drawable type via
      //  |
      //   ->  a call to our model vtable draw method, via
      //     |
      //      -> call or draw function for our data type.
      //
      friend void draw(drawable_t const& x, renderer const& r)
      { x.self->draw_(r); }
      
    private:
      
      // Drawable interface.
      //
      struct concept_t
      {
        virtual ~concept_t() = default;
        virtual concept_t* copy() const = 0;
        virtual void draw_(renderer const&) const = 0;
      };
      
      template<typename T>
      struct model : concept_t
      {
        // {} init breaks this. Why? (It is to do with struct/aggregate init).
        model(T x) : data(std::move(x)) { }
        concept_t* copy() const { return new model{*this}; }
        void draw_(renderer const& r) const { draw(data, r); }
        T data;
      };
      
      std::unique_ptr<concept_t> self;
    };
            
    // A scene. Basically, a vector of drawable_t.
    //
    //
    using scene_t = std::vector<drawable_t>;
    
    void draw(scene_t const& x, renderer const& r)
    { for (auto const& e: x) draw(e, r); }
    
    
  }
}

#endif /* DPJ_DRAWABLE_HH_ */
