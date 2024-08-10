#pragma once


// Helper type for the standard library visitor
template<class... Ts>
struct Overloaded : Ts... { using Ts::operator()...; };
