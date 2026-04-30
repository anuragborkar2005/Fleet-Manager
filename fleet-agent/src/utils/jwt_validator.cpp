#include "jwt_validator.hpp"

#include <exception>
#include <iostream>
#include <jwt-cpp/traits/nlohmann-json/defaults.h>
#include <jwt-cpp/traits/nlohmann-json/traits.h>

bool validate_jwt(const std::string &token, const std::string &secret)
{
    try
    {
        auto decoded = jwt::decoded_jwt<jwt::traits::nlohmann_json>(token);

        auto verifier =
            jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{secret})
                .with_issuer("auth0")
                .with_claim("user_id", jwt::claim(std::string("linux")));

        verifier.verify(decoded);
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Verification Failed" << e.what() << std::endl;
        return false;
    }
}