package com.example.acunote.security;

import io.jsonwebtoken.Claims;
import io.jsonwebtoken.Jwts;
import io.jsonwebtoken.SignatureAlgorithm;
import org.springframework.stereotype.Component;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;

@Component
public class JwtUtil {
// 密钥
    private final String SECRET_KEY =// 这里是密钥，很长很长很长
            "YnezArkdeVeryLongSecretKeyStringHereWithAtLeast64CharactersforSecurityAndMyNameIsYinXuezhou666AndILoveMinecraftwhenIwasYoung";
    private final long EXPIRATION = 86400000; // token过期时间24 小时
// 生成token
    public String generateToken(String username) {
        Map<String, Object> claims = new HashMap<>();
        return Jwts.builder()
                .setClaims(claims)
                .setSubject(username)
                .setIssuedAt(new Date(System.currentTimeMillis()))
                .setExpiration(new Date(System.currentTimeMillis() + EXPIRATION))
                .signWith(SignatureAlgorithm.HS512, SECRET_KEY)
                .compact();
    }
// 验证token
    public Boolean validateToken(String token, String username) {
        final String extractedUsername = extractUsername(token);
        return (extractedUsername.equals(username) && !isTokenExpired(token));
    }
// 获取token中的用户名
    public String extractUsername(String token) {
        final Claims claims = extractAllClaims(token);
        return claims.getSubject();
    }
// 判断token是否过期
    public Boolean isTokenExpired(String token) {
        final Date expiration = extractExpiration(token);
        return expiration.before(new Date());
    }
// 获取token中的所有信息
    private Claims extractAllClaims(String token) {
        return Jwts.parserBuilder()
                .setSigningKey(SECRET_KEY)
                .build()
                .parseClaimsJws(token)
                .getBody();
    }
// 获取token中的过期时间
    private Date extractExpiration(String token) {
        return extractAllClaims(token).getExpiration();
    }
}