#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>
#include <cassert>

using namespace std;

class Domain {
public:
    Domain(string domain) {
        reverse(domain.begin(), domain.end());
        domain_ = domain + '.';
    }

    bool operator==(const Domain& other) const {
        return other.domain_ == domain_;
    }

    bool operator<(const Domain& other) const {
        return domain_ < other.domain_;
    }

    bool IsSubdomain(const Domain& other) const {
        if (!(other.domain_.size() > domain_.size())) {
            return std::equal(other.domain_.begin(), other.domain_.end(), domain_.begin());
        }
        return false;
    }

    string ReturnDomain() const {
        return domain_;
    }

    string::const_iterator begin() const {
        return domain_.cbegin();
    }

    string::const_iterator end() const {
        return domain_.cend();
    }

private:
    string domain_;
};

class DomainChecker {
public:
    template<typename It>
    DomainChecker(const It begin, const It end) : domains_(begin, end) {
        if (!domains_.size()) {
            return;
        }
        sort(domains_.begin(), domains_.end(), [](const Domain& lhs, const Domain& rhs){
            return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
        });
        domains_.erase(unique(domains_.begin(), domains_.end(), [](const Domain& lhs, const Domain& rhs){
            return rhs.IsSubdomain(lhs);
        }), domains_.end());
    }

    bool IsForbidden(const Domain& other) {
        if (!domains_.size()) {
            return false;
        }
        auto it = upper_bound(domains_.begin(), domains_.end(), other, [](const Domain& lhs, const Domain& rhs){
            return lhs < rhs;
        });
        if (it != domains_.begin()) {
            it = prev(it);
            return other.IsSubdomain(*it);
        }
        return other.IsSubdomain(*it);
    }

private:
    vector<Domain> domains_;
};

vector<Domain> ReadDomains(istream& input, size_t count) {
    vector<Domain> result;
    result.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        string domain;
        getline(input, domain);
        result.push_back(Domain(domain));
    }

    return result;
}

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

void ReadDomainTest() {
    stringstream stream;

    stream << "5\n"s
              "habr.com\n"
              "leetcode.com\n"
              "codewars.com\n"
              "ru.wiki.net\n"
              "localhost\n";

    const std::vector<Domain> domains = ReadDomains(stream, ReadNumberOnLine<size_t>(stream));

    assert(domains[0] == Domain{"habr.com"});
    assert(domains[1] == Domain{"leetcode.com"});
    assert(domains[2] == Domain{"codewars.com"});
    assert(domains[3] == Domain{"ru.wiki.net"});
    assert(domains[4] == Domain{"localhost"});

}

void SubDomainTest() {
    assert(!Domain{"com"s}.IsSubdomain(Domain{"habr.com"s}));
    assert(Domain{"habr.com"s}.IsSubdomain(Domain{"habr.com"s}));
    assert(Domain{"habr.com"s}.IsSubdomain(Domain{"com"s}));
    assert(!Domain{"this_cool_domain.dotcom"s}.IsSubdomain(Domain{"com"s}));
}

void EqualDomainTest() {
    assert(Domain{"this_is_super_cool_and_long_and_FAT_domain.lol"} == Domain{"this_is_super_cool_and_long_and_FAT_domain.lol"});
    assert(Domain{"this.is.also.cool.domain.da"} == Domain{"this.is.also.cool.domain.da"});
    assert(!(Domain{"i_am_happy_domain.com"} == Domain{"i_am_happy_domain,com"}));
}

void DefaultTest() {
    stringstream stream;

    stream << "4\n"s
              "gdz.ru\n"
              "maps.me\n"
              "m.gdz.ru\n"
              "com\n";

    const std::vector<Domain> forbidden_domains = ReadDomains(stream, ReadNumberOnLine<size_t>(stream));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    assert(checker.IsForbidden(Domain{"gdz.ru"s}));
    assert(checker.IsForbidden(Domain{"gdz.com"s}));
    assert(checker.IsForbidden(Domain{"m.maps.me"s}));
    assert(checker.IsForbidden(Domain{"alg.m.gdz.ru"s}));
    assert(checker.IsForbidden(Domain{"maps.com"s}));
    assert(!checker.IsForbidden(Domain{"maps.ru"s}));
    assert(!checker.IsForbidden(Domain{"gdz.ua"s}));
}

void RunTest() {
    EqualDomainTest();
    ReadDomainTest();
    SubDomainTest();
    DefaultTest();
}

int main() {
    RunTest();
    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}
