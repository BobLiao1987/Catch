#include "internal/catch_stringref.h"

#include "internal/catch_suppress_warnings.h"

// !TBD always owning string implementation
namespace Catch {
    
    class String  {
    public:
        String() {}
    };
}

namespace Catch {
    

    
}

#include <string>

namespace Catch {

    // Implementation of test accessors
    auto rawCharData( StringRef const& stringRef ) -> char const* {
        return stringRef.m_data;
    }
    auto isOwned( StringRef const& stringRef ) -> bool {
        return stringRef.m_ownership == StringRef::Ownership::FullStringOwned;
    }
    auto isSubstring( StringRef const& stringRef ) -> bool {
        return stringRef.m_ownership == StringRef::Ownership::SubStringRef;
    }
    
    // Allow StringRefs to be converted to std::strings for printing
    // - this will become redundant when toString is reimplemented in terms
    // of String/StringRef
    inline auto toString( StringRef const& stringRef ) -> std::string {
        return std::string( rawCharData( stringRef ), stringRef.size() );
    }
}

#include "catch.hpp"

TEST_CASE( "StringRef" ) {
    
    using Catch::StringRef;
    
    SECTION( "Empty string" ) {
        StringRef empty;
        REQUIRE( empty.empty() );
        REQUIRE( empty.size() == 0 );
        REQUIRE( std::strcmp( empty.c_str(), "" ) == 0 );
    }

    SECTION( "From string literal" ) {
        StringRef s = "hello";
        REQUIRE( s.empty() == false );
        REQUIRE( s.size() == 5 );
        REQUIRE( isSubstring( s ) == false );

        auto rawChars = rawCharData( s );
        REQUIRE( std::strcmp( rawChars, "hello" ) == 0 );

        SECTION( "c_str() does not cause copy" ) {
            REQUIRE( isOwned( s ) == false );

            REQUIRE( s.c_str() == rawChars );

            REQUIRE( isOwned( s ) == false );
        }
    }
    SECTION( "From sub-string" ) {
        StringRef original = StringRef( "original string" ).substr(0, 8);
        REQUIRE( original == "original" );
        REQUIRE( isSubstring( original ) );
        REQUIRE( isOwned( original ) == false );
        
        original.c_str(); // Forces it to take ownership
        
        REQUIRE( isSubstring( original ) == false );
        REQUIRE( isOwned( original ) );
        
    }
    

    SECTION( "Substrings" ) {
        StringRef s = "hello world!";
        StringRef ss = s.substr(0, 5);

        SECTION( "zero-based substring" ) {
            REQUIRE( ss.empty() == false );
            REQUIRE( ss.size() == 5 );
            REQUIRE( std::strcmp( ss.c_str(), "hello" ) == 0 );
            REQUIRE( ss == "hello" );
        }
        SECTION( "c_str() causes copy" ) {
            REQUIRE( isSubstring( ss ) );
            REQUIRE( isOwned( ss ) == false );

            auto rawChars = rawCharData( ss );
            REQUIRE( rawChars == rawCharData( s ) ); // same pointer value
            REQUIRE( ss.c_str() != rawChars );
            
            REQUIRE( isSubstring( ss ) == false );
            REQUIRE( isOwned( ss ) );
            
            REQUIRE( rawCharData( ss ) != rawCharData( s ) ); // different pointer value
        }
        
        SECTION( "non-zero-based substring") {
            ss = s.substr( 6, 6 );
            REQUIRE( ss.size() == 6 );
            REQUIRE( std::strcmp( ss.c_str(), "world!" ) == 0 );
        }
        
        SECTION( "Pointer values of full refs should match" ) {
            StringRef s2 = s;
            REQUIRE( s.c_str() == s2.c_str() );
        }

        SECTION( "Pointer values of substring refs should not match" ) {
            REQUIRE( s.c_str() != ss.c_str() );
        }
    }
    
    SECTION( "Comparisons" ) {
        REQUIRE( StringRef("hello") == StringRef("hello") );
        REQUIRE( StringRef("hello") != StringRef("cello") );
    }
}
