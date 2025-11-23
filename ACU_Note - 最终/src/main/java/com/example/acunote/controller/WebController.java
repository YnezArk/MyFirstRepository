package com.example.acunote.controller;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;

@Controller
public class WebController {

    @GetMapping("/login")
    public String loginPage() {
        return "login";
    }

    @GetMapping("/register")
    public String registerPage() {
        return "register";
    }

    @GetMapping("/notes")
    public String notesPage() {
        return "notes";
    }

    @GetMapping("/")
    public String home() {
        return "redirect:/login";
    }
}