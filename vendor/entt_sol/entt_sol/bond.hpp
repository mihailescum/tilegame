/*
Creative Commons Legal Code

CC0 1.0 Universal

    CREATIVE COMMONS CORPORATION IS NOT A LAW FIRM AND DOES NOT PROVIDE
    LEGAL SERVICES. DISTRIBUTION OF THIS DOCUMENT DOES NOT CREATE AN
    ATTORNEY-CLIENT RELATIONSHIP. CREATIVE COMMONS PROVIDES THIS
    INFORMATION ON AN "AS-IS" BASIS. CREATIVE COMMONS MAKES NO WARRANTIES
    REGARDING THE USE OF THIS DOCUMENT OR THE INFORMATION OR WORKS
    PROVIDED HEREUNDER, AND DISCLAIMS LIABILITY FOR DAMAGES RESULTING FROM
    THE USE OF THIS DOCUMENT OR THE INFORMATION OR WORKS PROVIDED
    HEREUNDER.

Statement of Purpose

The laws of most jurisdictions throughout the world automatically confer
exclusive Copyright and Related Rights (defined below) upon the creator
and subsequent owner(s) (each and all, an "owner") of an original work of
authorship and/or a database (each, a "Work").

Certain owners wish to permanently relinquish those rights to a Work for
the purpose of contributing to a commons of creative, cultural and
scientific works ("Commons") that the public can reliably and without fear
of later claims of infringement build upon, modify, incorporate in other
works, reuse and redistribute as freely as possible in any form whatsoever
and for any purposes, including without limitation commercial purposes.
These owners may contribute to the Commons to promote the ideal of a free
culture and the further production of creative, cultural and scientific
works, or to gain reputation or greater distribution for their Work in
part through the use and efforts of others.

For these and/or other purposes and motivations, and without any
expectation of additional consideration or compensation, the person
associating CC0 with a Work (the "Affirmer"), to the extent that he or she
is an owner of Copyright and Related Rights in the Work, voluntarily
elects to apply CC0 to the Work and publicly distribute the Work under its
terms, with knowledge of his or her Copyright and Related Rights in the
Work and the meaning and intended legal effect of CC0 on those rights.

1. Copyright and Related Rights. A Work made available under CC0 may be
protected by copyright and related or neighboring rights ("Copyright and
Related Rights"). Copyright and Related Rights include, but are not
limited to, the following:

  i. the right to reproduce, adapt, distribute, perform, display,
     communicate, and translate a Work;
 ii. moral rights retained by the original author(s) and/or performer(s);
iii. publicity and privacy rights pertaining to a person's image or
     likeness depicted in a Work;
 iv. rights protecting against unfair competition in regards to a Work,
     subject to the limitations in paragraph 4(a), below;
  v. rights protecting the extraction, dissemination, use and reuse of data
     in a Work;
 vi. database rights (such as those arising under Directive 96/9/EC of the
     European Parliament and of the Council of 11 March 1996 on the legal
     protection of databases, and under any national implementation
     thereof, including any amended or successor version of such
     directive); and
vii. other similar, equivalent or corresponding rights throughout the
     world based on applicable law or treaty, and any national
     implementations thereof.

2. Waiver. To the greatest extent permitted by, but not in contravention
of, applicable law, Affirmer hereby overtly, fully, permanently,
irrevocably and unconditionally waives, abandons, and surrenders all of
Affirmer's Copyright and Related Rights and associated claims and causes
of action, whether now known or unknown (including existing as well as
future claims and causes of action), in the Work (i) in all territories
worldwide, (ii) for the maximum duration provided by applicable law or
treaty (including future time extensions), (iii) in any current or future
medium and for any number of copies, and (iv) for any purpose whatsoever,
including without limitation commercial, advertising or promotional
purposes (the "Waiver"). Affirmer makes the Waiver for the benefit of each
member of the public at large and to the detriment of Affirmer's heirs and
successors, fully intending that such Waiver shall not be subject to
revocation, rescission, cancellation, termination, or any other legal or
equitable action to disrupt the quiet enjoyment of the Work by the public
as contemplated by Affirmer's express Statement of Purpose.

3. Public License Fallback. Should any part of the Waiver for any reason
be judged legally invalid or ineffective under applicable law, then the
Waiver shall be preserved to the maximum extent permitted taking into
account Affirmer's express Statement of Purpose. In addition, to the
extent the Waiver is so judged Affirmer hereby grants to each affected
person a royalty-free, non transferable, non sublicensable, non exclusive,
irrevocable and unconditional license to exercise Affirmer's Copyright and
Related Rights in the Work (i) in all territories worldwide, (ii) for the
maximum duration provided by applicable law or treaty (including future
time extensions), (iii) in any current or future medium and for any number
of copies, and (iv) for any purpose whatsoever, including without
limitation commercial, advertising or promotional purposes (the
"License"). The License shall be deemed effective as of the date CC0 was
applied by Affirmer to the Work. Should any part of the License for any
reason be judged legally invalid or ineffective under applicable law, such
partial invalidity or ineffectiveness shall not invalidate the remainder
of the License, and in such case Affirmer hereby affirms that he or she
will not (i) exercise any of his or her remaining Copyright and Related
Rights in the Work or (ii) assert any associated claims and causes of
action with respect to the Work, in either case contrary to Affirmer's
express Statement of Purpose.

4. Limitations and Disclaimers.

 a. No trademark or patent rights held by Affirmer are waived, abandoned,
    surrendered, licensed or otherwise affected by this document.
 b. Affirmer offers the Work as-is and makes no representations or
    warranties of any kind concerning the Work, express, implied,
    statutory or otherwise, including without limitation warranties of
    title, merchantability, fitness for a particular purpose, non
    infringement, or the absence of latent or other defects, accuracy, or
    the present or absence of errors, whether or not discoverable, all to
    the greatest extent permissible under applicable law.
 c. Affirmer disclaims responsibility for clearing rights of other persons
    that may apply to the Work or any use thereof, including without
    limitation any person's Copyright and Related Rights in the Work.
    Further, Affirmer disclaims responsibility for obtaining any necessary
    consents, permissions or other rights required for any use of the
    Work.
 d. Affirmer understands and acknowledges that Creative Commons is not a
    party to this document and has no duty or obligation with respect to
    this CC0 or use of the Work.
*/

#ifndef __ENTT_SOL__BOND_HPP__
#define __ENTT_SOL__BOND_HPP__

#include <set>

#include <entt/entity/registry.hpp>
#include <entt/entity/runtime_view.hpp>
#include "meta_helper.hpp"

// Disable automagical creation of meta functions, because 'registry.size()' is deprecated.
// See https://sol2.readthedocs.io/en/latest/api/usertype.html?highlight=meta%20function#automagical-usertypes
namespace sol
{
  template <>
  struct is_automagical<entt::registry> : std::false_type
  {
  };
}

namespace entt_sol
{
  template <typename Component>
  inline auto is_valid(const entt::registry *registry, entt::entity entity)
  {
    assert(registry);
    return registry->valid(entity);
  }
  template <typename Component>
  inline auto emplace_component(entt::registry *registry, entt::entity entity,
                                const sol::table &instance, sol::this_state s)
  {
    assert(registry);
    if constexpr (std::is_empty_v<Component>)
    {
      registry->emplace<Component>(entity);
      return sol::reference(s, sol::lua_nil);
    }
    else
    {
      auto &comp = registry->emplace_or_replace<Component>(
          entity, instance.valid() ? instance.as<Component>() : Component{});
      return sol::make_reference(s, std::ref(comp));
    }
  }
  template <typename Component>
  inline auto get_component(entt::registry *registry, entt::entity entity,
                            sol::this_state s)
  {
    assert(registry);
    if constexpr (std::is_empty_v<Component>)
    {
      return sol::reference(s, sol::lua_nil);
    }
    else
    {
      auto &comp = registry->get_or_emplace<Component>(entity);
      return sol::make_reference(s, std::ref(comp));
    }
  }
  template <typename Component>
  inline bool has_component(entt::registry *registry, entt::entity entity)
  {
    assert(registry);
    return registry->any_of<Component>(entity);
  }
  template <typename Component>
  inline auto remove_component(entt::registry *registry, entt::entity entity)
  {
    assert(registry);
    return registry->remove<Component>(entity);
  }
  template <typename Component>
  inline void clear_component(entt::registry *registry)
  {
    assert(registry);
    registry->clear<Component>();
  }

  template <typename Component>
  inline void register_meta_component()
  {
    using namespace entt::literals;

    entt::meta<Component>()
        .template func<&is_valid<Component>>("valid"_hs)
        .template func<&emplace_component<Component>>("emplace"_hs)
        .template func<&get_component<Component>>("get"_hs)
        .template func<&has_component<Component>>("has"_hs)
        .template func<&clear_component<Component>>("clear"_hs)
        .template func<&remove_component<Component>>("remove"_hs);
  }

  inline auto collect_types(const sol::variadic_args &va)
  {
    std::set<entt::id_type> types;
    std::transform(va.cbegin(), va.cend(), std::inserter(types, types.begin()),
                   [](const auto &obj)
                   { return deduce_type(obj); });
    return types;
  }

  inline sol::table open_registry(sol::this_state s)
  {
    // To create a registry inside a script: entt.registry.new()

    sol::state_view lua{s};
    auto entt_module = lua["entt"].get_or_create<sol::table>();

    // clang-format off
  entt_module.new_usertype<entt::runtime_view>("runtime_view",
    sol::no_constructor,

    "size_hint", &entt::runtime_view::size_hint,
    "contains", &entt::runtime_view::contains,
    "each",
      [](const entt::runtime_view &self, const sol::function &callback) {
        if (callback.valid()) {
          for (auto entity : self) callback(entity);
        }
      }
  );

  using namespace entt::literals;

  entt_module.new_usertype<entt::registry>("registry",
    sol::meta_function::construct,
    sol::factories([]{ return entt::registry{}; }),

    "size", [](const entt::registry &self) {
      return self.storage<entt::entity>()->size();
    },
    sol::meta_function::length, [](const entt::registry &self) {
      return self.storage<entt::entity>()->size();
    },
    "alive", [](const entt::registry &self) {
      return self.storage<entt::entity>()->in_use();
    },

    "valid", &entt::registry::valid,
    "current", &entt::registry::current,

    "create", [](entt::registry &self) { return self.create(); },
    "destroy",
      [](entt::registry &self, entt::entity entity) {
        return self.destroy(entity);
      },

    "emplace",
      [](entt::registry &self, entt::entity entity, const sol::table &comp,
         sol::this_state s) -> sol::object {
        if (!comp.valid()) return sol::lua_nil_t{};
        const auto maybe_any = invoke_meta_func(get_type_id(comp), "emplace"_hs,
          &self, entity, comp, s);
        return maybe_any ? maybe_any.cast<sol::reference>() : sol::lua_nil_t{};
      },
    "remove",
      [](entt::registry &self, entt::entity entity, const sol::object &type_or_id) {
        const auto maybe_any =
          invoke_meta_func(deduce_type(type_or_id), "remove"_hs, &self, entity);
        return maybe_any ? maybe_any.cast<size_t>() : 0;
      },
    "has",
      [](entt::registry &self, entt::entity entity, const sol::object &type_or_id) {
        const auto maybe_any =
          invoke_meta_func(deduce_type(type_or_id), "has"_hs, &self, entity);
        return maybe_any ? maybe_any.cast<bool>() : false;
      },
    "any_of",
      [](const sol::table &self, entt::entity entity, const sol::variadic_args &va) {
        const auto types = collect_types(va);
        const auto has = self["has"].get<sol::function>();
        return std::any_of(types.cbegin(), types.cend(),
          [&](auto type_id) { return has(self, entity, type_id).template get<bool>(); }
        );
      },
    "get",
      [](entt::registry &self, entt::entity entity, const sol::object &type_or_id,
         sol::this_state s) {
      const auto maybe_any =
        invoke_meta_func(deduce_type(type_or_id), "get"_hs,
          &self, entity, s);
      return maybe_any ? maybe_any.cast<sol::reference>() : sol::lua_nil_t{};
    },
    "clear",
      sol::overload(
        &entt::registry::clear<>,
        [](entt::registry &self, sol::object type_or_id) {
          invoke_meta_func(deduce_type(type_or_id), "clear"_hs, &self);
        }
      ),

    "orphan", &entt::registry::orphan,

    "runtime_view",
      [](entt::registry &self, const sol::variadic_args &va) {
        const auto types = collect_types(va);
        
        auto view = entt::runtime_view{};
        for (auto &&[componentId, storage]: self.storage()) {
          if (types.find(componentId) != types.cend()) {
            view.iterate(storage);
          }
        }
        return view;
      }
  );
    // clang-format on

    return entt_module;
  }
}
#endif