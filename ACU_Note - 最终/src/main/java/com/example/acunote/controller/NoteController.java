package com.example.acunote.controller;

import com.example.acunote.model.Note;
import com.example.acunote.model.User;
import com.example.acunote.repository.NoteRepository;
import com.example.acunote.repository.UserRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/notes")
public class NoteController {
// 笔记
    @Autowired
    private NoteRepository noteRepository;
// 用户
    @Autowired
    private UserRepository userRepository;
// 创建笔记
    @PostMapping
    public Note createNote(@RequestBody Note note) {
        String username = SecurityContextHolder.getContext().getAuthentication().getName();
        User owner = userRepository.findByUsername(username).orElse(null);
        note.setOwner(owner);
        return noteRepository.save(note);
    }
// 获取我的笔记
    @GetMapping
    public List<Note> getMyNotes() {
        String username = SecurityContextHolder.getContext().getAuthentication().getName();
        User user = userRepository.findByUsername(username).orElse(null);
        return noteRepository.findByOwner_Id(user.getId());
    }
// 获取笔记详情
    @GetMapping("/{id}")
    public Note getNoteById(@PathVariable Long id) {
        String username = SecurityContextHolder.getContext().getAuthentication().getName();
        User user = userRepository.findByUsername(username).orElse(null);
        Note note = noteRepository.findById(id).orElse(null);
        if (note != null && note.getOwner().getId().equals(user.getId())) {
            return note;
        }
        throw new RuntimeException("Access denied");
    }
// 更新笔记
    @PutMapping("/{id}")
    public Note updateNote(@PathVariable Long id, @RequestBody Note note) {
        String username = SecurityContextHolder.getContext().getAuthentication().getName();
        User user = userRepository.findByUsername(username).orElse(null);
        Note existingNote = noteRepository.findById(id).orElse(null);
        if (existingNote == null || !existingNote.getOwner().getId().equals(user.getId())) {
            throw new RuntimeException("Access denied");
        }
        existingNote.setTitle(note.getTitle());
        existingNote.setContent(note.getContent());
        return noteRepository.save(existingNote);
    }
// 删除笔记
    @DeleteMapping("/{id}")
    public void deleteNote(@PathVariable Long id) {
        String username = SecurityContextHolder.getContext().getAuthentication().getName();
        User user = userRepository.findByUsername(username).orElse(null);
        Note note = noteRepository.findById(id).orElse(null);
        if (note != null && note.getOwner().getId().equals(user.getId())) {
            noteRepository.deleteById(id);
        } else {
            throw new RuntimeException("Access denied");
        }
    }
}