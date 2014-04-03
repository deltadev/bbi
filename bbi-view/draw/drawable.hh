//
//  polymorphic.hh
//  gn-sean-parent
//
//  Created by Daniel James on 01/03/2014.
//  Copyright (c) 2014 Daniel James. All rights reserved.
//

#ifndef gn_sean_parent_polymorphic_hh
#define gn_sean_parent_polymorphic_hh

#include <iostream>

#include <string>

#include <vector>

template<typename T>
void draw(T const& x, std::ostream& os, std::size_t position)
{ os << std::string(position, ' ') << x << '\n'; }

class object_t
{
public:
  
  template<typename T>
  object_t(T x) : self(new model<T>{std::move(x)})
  { std::cout << "ctor\n"; }
  
  
  object_t(object_t const& x) : self(x.self->copy())
  { std::cout << "copy\n"; }
  object_t(object_t&&) noexcept = default;
  
  
  object_t& operator=(object_t const& x)
  { object_t tmp{x}; *this = std::move(tmp); return *this; }
  object_t& operator=(object_t&&) noexcept = default;
  
  
  friend void draw(object_t const& x, std::ostream& os, std::size_t position)
  { x.self->draw_(os, position); }
  
private:
  struct concept_t
  {
    virtual ~concept_t() = default;
    virtual concept_t* copy() const = 0;
    virtual void draw_(std::ostream&, std::size_t) const = 0;
  };
  
  template<typename T>
  struct model : concept_t
  {
    // {} init breaks this. Why? (It is to do with struct/aggregate init).
    model(T x) : data(std::move(x)) { }
    concept_t* copy() const { return new model{*this}; }
    void draw_(std::ostream& os, std::size_t position) const
    { draw(data, os, position); }
    T data;
  };
  
  std::unique_ptr<concept_t> self;
};

using document_t = std::vector<object_t>;

void draw(document_t const& x, std::ostream& os, std::size_t position)
{
  os << std::string(position, ' ') << "<document>\n";
  for (auto const& e: x) draw(e, os, 2);
  os << std::string(position, ' ') << "</document>\n";
}

#endif
