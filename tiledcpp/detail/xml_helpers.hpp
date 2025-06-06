#pragma once
#include <RapidXML/rapidxml.hpp>

namespace tpp::detail
{

template <typename F>
void foreachChildNode(const rapidxml::xml_node<char>* parent_node, const char* name, F&& function)
{
    for (auto child = parent_node->first_node(name); child != nullptr; child = child->next_sibling(name))
    {
        function(child);
    }
}

}