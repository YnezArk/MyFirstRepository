package com.example.acunote.model;

import jakarta.persistence.*;
import lombok.Data;

import java.time.LocalDateTime;

@Entity
@Table(name = "notes")
@Data

public class Note {
    // id, title, content, created_at, owner_id
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    //标题
    private String title;
    // 内容
    private String content;
    // 创建时间
    @Column(updatable = false)
    private LocalDateTime created_at = LocalDateTime.now();
    // 用户
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "owner_id", nullable = false)
    private User owner;
}