/*
 * Copyright (C) 2008 Search Solution Corporation. All rights reserved by Search Solution.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

/*
 * load_class_registry.hpp - class registry for server loaddb
 */

#ifndef _LOAD_CLASS_REGISTRY_HPP_
#define _LOAD_CLASS_REGISTRY_HPP_

#include "load_common.hpp"
#include "object_representation_sr.h"
#include "storage_common.h"

#include <mutex>
#include <unordered_map>
#include <vector>

namespace cubload
{

  struct attribute
  {
    attribute () = delete; // Not DefaultConstructible
    attribute (ATTR_ID attr_id, std::string attr_name, or_attribute *attr_repr);

    attribute (attribute &&other) = delete; // Not MoveConstructible
    attribute (const attribute &copy) = delete; // Not CopyConstructible

    attribute &operator= (attribute &&other) = delete; // Not MoveAssignable
    attribute &operator= (const attribute &copy) = delete; // Not CopyAssignable

    ATTR_ID m_attr_id;
    std::string m_attr_name;
    or_attribute *m_attr_repr;
  };

  class class_entry
  {
    public:
      class_entry () = delete; // Not DefaultConstructible
      class_entry (std::string &class_name, OID &class_oid, class_id clsid, std::vector<const attribute *> &attributes);
      ~class_entry ();

      class_entry (class_entry &&other) = delete; // Not MoveConstructible
      class_entry (const class_entry &copy) = delete; // Not CopyConstructible
      class_entry &operator= (class_entry &&other) = delete; // Not MoveAssignable
      class_entry &operator= (const class_entry &copy) = delete; // Not CopyAssignable

      const OID &get_class_oid () const;
      const attribute &get_attribute (std::size_t index) const;

    private:
      class_id m_clsid;
      OID m_class_oid;
      std::string m_class_name;
      std::vector<const attribute *> m_attributes;
  };

  class class_registry
  {
    public:
      class_registry ();
      ~class_registry ();

      class_registry (class_registry &&other) = delete; // Not MoveConstructible
      class_registry (const class_registry &copy) = delete; // Not CopyConstructible

      class_registry &operator= (class_registry &&other) = delete; // Not MoveAssignable
      class_registry &operator= (const class_registry &copy) = delete; // Not CopyAssignable

      const class_entry *get_class_entry (class_id clsid);
      void register_class (const char *class_name, class_id clsid, OID class_oid,
			   std::vector<const attribute *> &attributes);

    private:
      std::mutex m_mutex;
      std::unordered_map<class_id, const class_entry *> m_class_by_id;

      const class_entry *get_class_entry_without_lock (class_id clsid) ;
  };

} // namespace cubload

#endif /* _LOAD_CLASS_REGISTRY_HPP_ */
