package com.example.acunote.controller;

import com.example.acunote.model.User;
import com.example.acunote.repository.UserRepository;
import com.example.acunote.security.JwtUtil;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.security.authentication.AuthenticationManager;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.web.bind.annotation.*;

import java.util.Map;

@RestController
@RequestMapping("/api")

public class AuthController {
// 用户注册和登录
    @Autowired
    private UserRepository userRepository;
// 认证管理
    @Autowired
    private AuthenticationManager authenticationManager;
// JWT工具
    @Autowired
    private JwtUtil jwtUtil;
// 用户注册
    @PostMapping("/register")
    // @RequestBody注解表示接收JSON数据
    public ResponseEntity<Map<String, String>> register(@RequestBody User user) {
        if (userRepository.findByUsername(user.getUsername()).isPresent()) {
            return ResponseEntity.badRequest().body(Map.of("error", "Username already exists"));
        }
        userRepository.save(user);
        return ResponseEntity.ok(Map.of("message", "User registered successfully"));
    }
// 用户登录
    @PostMapping("/login")
    // @RequestBody注解表示接收JSON数据
    public ResponseEntity<Map<String, String>> login(@RequestBody Map<String, String> credentials) {
        try {//这个地方搞了好久，一直提示密码错误，catch了多次才发现是必须返回空列表而不是返回null
            String username = credentials.get("username");
            String password = credentials.get("password");

            System.out.println(">>> Login attempt: " + username);

            // 触发认证
            authenticationManager.authenticate(
                    new UsernamePasswordAuthenticationToken(username, password)
            );

            String token = jwtUtil.generateToken(username);
            return ResponseEntity.ok(Map.of("token", token, "message", "Login successful"));
        } catch (Exception e) {
            System.out.println(">>> Login failed: " + e.getClass().getSimpleName() + " - " + e.getMessage());
            e.printStackTrace();
            return ResponseEntity.status(401).body(Map.of("error", "Invalid username or password"));
        }

    }
}