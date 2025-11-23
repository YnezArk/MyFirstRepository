package com.example.acunote.security;

import jakarta.servlet.FilterChain;
import jakarta.servlet.ServletException;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.web.authentication.WebAuthenticationDetailsSource;
import org.springframework.stereotype.Component;
import org.springframework.web.filter.OncePerRequestFilter;

import java.io.IOException;

@Component
public class JwtFilter extends OncePerRequestFilter {
// JWT过滤器
    @Autowired
    private JwtUtil jwtUtil;
// 用户详情服务
    @Autowired
    private CustomUserDetailsService customUserDetailsService;
// 过滤器
    @Override
    protected void doFilterInternal(HttpServletRequest request, HttpServletResponse response, FilterChain chain)
            throws ServletException, IOException {// 获取请求头中的JWT令牌
        System.out.println(">>> Request URI: " + request.getRequestURI());
        System.out.println(">>> Authorization Header: " + request.getHeader("Authorization"));

        String requestTokenHeader = request.getHeader("Authorization");
// 用户名
        String username = null;
        // JWT令牌
        String jwtToken = null;
// 从请求头中获取JWT令牌
        if (requestTokenHeader != null && requestTokenHeader.startsWith("Bearer ")) {
            jwtToken = requestTokenHeader.substring(7);
            try {
                username = jwtUtil.extractUsername(jwtToken);
            } catch (Exception e) {
                System.out.println("JWT Token is invalid");
            }
        } else {
            System.out.println("JWT Token does not start with Bearer");
        }
// 验证JWT令牌
        if (username != null && SecurityContextHolder.getContext().getAuthentication() == null) {
            UserDetails userDetails = this.customUserDetailsService.loadUserByUsername(username);
// 验证令牌
            if (jwtUtil.validateToken(jwtToken, username)) {
                UsernamePasswordAuthenticationToken usernamePasswordAuthenticationToken =
                        new UsernamePasswordAuthenticationToken(userDetails, null, userDetails.getAuthorities());
                usernamePasswordAuthenticationToken.setDetails(new WebAuthenticationDetailsSource().buildDetails(request));
                SecurityContextHolder.getContext().setAuthentication(usernamePasswordAuthenticationToken);
            }
        }
        chain.doFilter(request, response);
    }
}