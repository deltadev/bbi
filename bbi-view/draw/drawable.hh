#ifndef DPJ_DRAWABLE_HH_
#define DPJ_DRAWABLE_HH_

#include <string>
#include <vector>

#include "renderer.hh"

namespace dpj
{
  namespace gl
  {
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
      drawable_t(T x) : self{new model<T>{std::move(x)}} { }
      drawable_t(drawable_t const& x) : self{x.self->copy()} { }
      drawable_t(drawable_t&&) noexcept = default;
      
      
      // Assignment
      //
      drawable_t& operator=(drawable_t&&) noexcept = default;
      drawable_t& operator=(drawable_t const& x)
      { drawable_t tmp{x}; *this = std::move(tmp); return *this; }
      
      // void draw(drawable_t const&, renderer const&)
      //
      //   - calls the type-erased drawable.
      //
      friend void draw(drawable_t const& x, renderer const& r)
      { x.self->draw_(r); }

      //
      //
      friend void update(drawable_t& x) { x.self->update_(); }
      
    private:
      
      // Drawable interface.
      //
      struct concept_t
      {
        virtual ~concept_t() = default;
        virtual concept_t* copy() const = 0;
        virtual void draw_(renderer const&) const = 0;
        virtual void update_() = 0;

      };
      
      template<typename T>
      struct model : concept_t
      {
        model(T x) : data{std::move(x)} { }
        concept_t* copy() const { return new model{*this}; }
        void draw_(renderer const& r) const { draw(data, r); }
        void update_() { update(data); }
        T data;
      };
      
      std::unique_ptr<concept_t> self;
    };
  }
}

#endif /* DPJ_DRAWABLE_HH_ */
