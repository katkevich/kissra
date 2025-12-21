import std;
import kissra;

int main() {
    std::array arr = { 1, 2, 3 };
    auto iter = kissra::all(arr) //
                    .filter(kissra::fn::odd)
                    .transform(kissra::fn::to_chars);

    while (auto s = iter.next()) {
        std::cout << *s << "\n";
    }
    return 0;
}