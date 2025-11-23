package com.example.acunote.model;

import jakarta.persistence.*;
import lombok.Data;

@Entity
@Table(name = "users")
@Data
public class User {
    // 用户表
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
// 用户名
    @Column(unique = true, nullable = false)
    private String username;
// 密码
    @Column(nullable = false)
    private String password;
}