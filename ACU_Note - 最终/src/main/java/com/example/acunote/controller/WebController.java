package com.example.acunote.controller;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;

@Controller
public class WebController {
// Web页面控制器
    // 登录页面
    @GetMapping("/login")
    public String loginPage() {
        return "login";
    }
// 注册页面
    @GetMapping("/register")
    public String registerPage() {
        return "register";
    }
// 笔记页面
    @GetMapping("/notes")
    public String notesPage() {
        return "notes";
    }
// 首页
    @GetMapping("/")
    public String home() {
        return "redirect:/login";
    }
}